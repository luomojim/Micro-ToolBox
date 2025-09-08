#include <stdio.h>
#include <windows.h>
#include <tchar.h>  // ����TCHAR��غ���
#include <tlhelp32.h>  // ����ö����غ���

// ������������ֹ���� explorer.exe ����
BOOL TerminateAllExplorers();
// �������������� explorer.exe
BOOL StartExplorer();

BOOL TerminateAllExplorers() 
    {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("�������̿���ʧ�ܣ�������: %lu\n", GetLastError());
        return FALSE;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);  // �����ʼ���ṹ���С

    // ���������б�
    if (!Process32First(hSnapshot, &pe32)) {
        printf("��������ʧ�ܣ�������: %lu\n", GetLastError());
        CloseHandle(hSnapshot);
        return FALSE;
    }

    BOOL bFound = FALSE;
    do {
        // ���������Ƿ�Ϊ "explorer.exe"�������ִ�Сд��
        if (_wcsicmp(pe32.szExeFile, L"explorer.exe") == 0) {
            bFound = TRUE;

            // �򿪽��̾������Ҫ��ֹȨ�ޣ�
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
            if (hProcess == NULL) {
                printf("�򿪽��� %lu ʧ�ܣ�������: %lu\n", pe32.th32ProcessID, GetLastError());
                continue;
            }

            // ��ֹ����
            if (!TerminateProcess(hProcess, 0)) {
                printf("��ֹ���� %lu ʧ�ܣ�������: %lu\n", pe32.th32ProcessID, GetLastError());
                CloseHandle(hProcess);
                continue;
            }

            printf("����ֹ���� %lu��PID: %lu��\n", pe32.szExeFile, pe32.th32ProcessID);
            CloseHandle(hProcess);
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return bFound;  // ��������ҵ�һ�� explorer �����򷵻� TRUE
}

// ���� explorer.exe
BOOL StartExplorer() {
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFO);  // �����ʼ���ṹ���С

    // explorer.exe �ı�׼·�����ɸ�����Ҫ�޸ģ�
    WCHAR szPath[] = L"C:\\Windows\\explorer.exe";

    // ��������
    BOOL bSuccess = CreateProcessW(
        szPath,                // ��ִ���ļ�·��
        NULL,                  // �����в�����NULL ��ʾʹ��·������Ĳ�����
        NULL,                  // ���̾����ȫ����
        NULL,                  // �߳̾����ȫ����
        FALSE,                 // ���̳и����̵ľ��
        0,                     // ������־��������Ҫ��
        NULL,                  // ����������ʹ�ø����̵ģ�
        NULL,                  // ����Ŀ¼��ʹ�ø����̵ģ�
        &si,                   // ������Ϣ
        &pi                    // ������Ϣ
    );

    if (!bSuccess) {
        printf("CreateProcess ʧ�ܣ�������: %lu\n", GetLastError());
        return FALSE;
    }

    // �رս��̺��߳̾����������Դй©��
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return TRUE;
}

int main()
{
    // ��ȡ��ǰ�û�����ʹ�ö��ֽڰ汾����Unicode���⣩
    char username[256] = { 0 };
    DWORD usernameSize = GetEnvironmentVariableA("USERNAME", username, sizeof(username));  // ANSI�汾
    if (usernameSize == 0 || usernameSize >= sizeof(username))
    {
        fprintf(stderr, "��ȡ�û���ʧ�ܣ�������: %lu", GetLastError());
            return 1;
    }

    // ����ͼ�껺���ļ�·��������·����ʹ�ð�ȫ��snprintf��
    char filename[512] = { 0 };  // �㹻��Ļ�����
    int pathLen = _snprintf_s(filename, sizeof(filename), _TRUNCATE,
        "C:\\Users\\%s\\AppData\\Local\\IconCache.db",
        username);
    if (pathLen < 0)  // ����ʽ���Ƿ�ʧ��
    {
        fprintf(stderr, "����·��ʧ��");
            return 1;
    }

    printf("Ŀ���ļ�·��: %s", filename);

    // ����ɾ���ļ���������Ҫ����ԱȨ�ޣ�
    if (remove(filename) == 0)
    {
        printf("�ļ� %s ɾ���ɹ���", filename);
    }
    else
    {
        fprintf(stderr, "ɾ���ļ�ʧ�ܣ�������: %lu", GetLastError());  // ������������
    }

    printf("����������Դ������...\n");

    // ����1����ֹ�������е� explorer.exe ����
    if (!TerminateAllExplorers()) 
    {
        printf("��ֹ explorer ʧ�ܣ�������: %lu\n", GetLastError());
        return 1;
    }

    // ����2�������µ� explorer.exe
    if (!StartExplorer()) 
    {
        printf("���� explorer ʧ�ܣ�������: %lu\n", GetLastError());
        return 1;
    }

    printf("��Դ�����������ɹ���\n");

    return 0;
}