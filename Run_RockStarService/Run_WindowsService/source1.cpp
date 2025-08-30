#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

void StartService(const char *name)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	
	if (hSCManager == NULL) 
	{
		printf("�޷��򿪷�����ƹ��������������: %lu\n", GetLastError());
		return;
	}

	hService = OpenServiceA(hSCManager,name,SERVICE_START);


	if (!StartService(hService, 0, NULL)) 
	{
		printf("��������ʧ�ܣ���������Ѿ����������Ի�����r��ƽ̨���������: %lu\n", GetLastError());
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return;
	}

	printf("�����ѳɹ�����,��������r��ƽ̨\n");


	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
}

int main()
{
	const char *name = "Rockstar Service";
	StartService(name);							//��������
	Sleep(5000);


	// ��ȡ��ǰ�û���
	TCHAR username[256] = { 0 };
	DWORD usernameSize = GetEnvironmentVariable(TEXT("USERNAME"), username, sizeof(username) / sizeof(TCHAR));
	if (usernameSize == 0 || usernameSize >= sizeof(username) / sizeof(TCHAR)) {
		_tprintf(TEXT("��ȡ�û���ʧ�ܣ�������: %lu\n"), GetLastError());
		return 1;
	}

	// ����Rockstar Games·��
	TCHAR rockstarDir[MAX_PATH] = { 0 };
	_stprintf_s(rockstarDir, sizeof(rockstarDir) / sizeof(TCHAR),
		TEXT("C:\\Users\\%s\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Rockstar Games"),username);

	// ������ݷ�ʽ����·������"Rockstar Games Launcher.lnk"�滻Ϊʵ�ʿ�ݷ�ʽ���ƣ�
	TCHAR lnkPath[MAX_PATH] = { 0 };
	_stprintf_s(lnkPath, sizeof(lnkPath) / sizeof(TCHAR),
		TEXT("%s\\Rockstar Games Launcher.lnk"), rockstarDir);

	// ����
	HINSTANCE hInstance = ShellExecute(NULL,TEXT("open"),lnkPath,NULL,rockstarDir,SW_SHOWNORMAL);

	if ((INT_PTR)hInstance <= 32) {
		_tprintf(TEXT("����ʧ�ܣ�������: %lu\n"), GetLastError());
		return 1;
	}
	
	return 0;
}