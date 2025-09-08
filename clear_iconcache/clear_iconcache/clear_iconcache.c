#include <stdio.h>
#include <windows.h>
#include <tchar.h>  // 包含TCHAR相关函数
#include <tlhelp32.h>  // 进程枚举相关函数

// 函数声明：终止所有 explorer.exe 进程
BOOL TerminateAllExplorers();
// 函数声明：启动 explorer.exe
BOOL StartExplorer();

BOOL TerminateAllExplorers() 
    {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("创建进程快照失败！错误码: %lu\n", GetLastError());
        return FALSE;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);  // 必须初始化结构体大小

    // 遍历进程列表
    if (!Process32First(hSnapshot, &pe32)) {
        printf("遍历进程失败！错误码: %lu\n", GetLastError());
        CloseHandle(hSnapshot);
        return FALSE;
    }

    BOOL bFound = FALSE;
    do {
        // 检查进程名是否为 "explorer.exe"（不区分大小写）
        if (_wcsicmp(pe32.szExeFile, L"explorer.exe") == 0) {
            bFound = TRUE;

            // 打开进程句柄（需要终止权限）
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
            if (hProcess == NULL) {
                printf("打开进程 %lu 失败！错误码: %lu\n", pe32.th32ProcessID, GetLastError());
                continue;
            }

            // 终止进程
            if (!TerminateProcess(hProcess, 0)) {
                printf("终止进程 %lu 失败！错误码: %lu\n", pe32.th32ProcessID, GetLastError());
                CloseHandle(hProcess);
                continue;
            }

            printf("已终止进程 %lu（PID: %lu）\n", pe32.szExeFile, pe32.th32ProcessID);
            CloseHandle(hProcess);
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return bFound;  // 如果至少找到一个 explorer 进程则返回 TRUE
}

// 启动 explorer.exe
BOOL StartExplorer() {
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFO);  // 必须初始化结构体大小

    // explorer.exe 的标准路径（可根据需要修改）
    WCHAR szPath[] = L"C:\\Windows\\explorer.exe";

    // 启动进程
    BOOL bSuccess = CreateProcessW(
        szPath,                // 可执行文件路径
        NULL,                  // 命令行参数（NULL 表示使用路径本身的参数）
        NULL,                  // 进程句柄安全属性
        NULL,                  // 线程句柄安全属性
        FALSE,                 // 不继承父进程的句柄
        0,                     // 创建标志（无特殊要求）
        NULL,                  // 环境变量（使用父进程的）
        NULL,                  // 工作目录（使用父进程的）
        &si,                   // 启动信息
        &pi                    // 进程信息
    );

    if (!bSuccess) {
        printf("CreateProcess 失败！错误码: %lu\n", GetLastError());
        return FALSE;
    }

    // 关闭进程和线程句柄（避免资源泄漏）
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return TRUE;
}

int main()
{
    // 获取当前用户名（使用多字节版本避免Unicode问题）
    char username[256] = { 0 };
    DWORD usernameSize = GetEnvironmentVariableA("USERNAME", username, sizeof(username));  // ANSI版本
    if (usernameSize == 0 || usernameSize >= sizeof(username))
    {
        fprintf(stderr, "获取用户名失败，错误码: %lu", GetLastError());
            return 1;
    }

    // 构造图标缓存文件路径（修正路径并使用安全的snprintf）
    char filename[512] = { 0 };  // 足够大的缓冲区
    int pathLen = _snprintf_s(filename, sizeof(filename), _TRUNCATE,
        "C:\\Users\\%s\\AppData\\Local\\IconCache.db",
        username);
    if (pathLen < 0)  // 检查格式化是否失败
    {
        fprintf(stderr, "构造路径失败");
            return 1;
    }

    printf("目标文件路径: %s", filename);

    // 尝试删除文件（可能需要管理员权限）
    if (remove(filename) == 0)
    {
        printf("文件 %s 删除成功。", filename);
    }
    else
    {
        fprintf(stderr, "删除文件失败，错误码: %lu", GetLastError());  // 输出具体错误码
    }

    printf("正在重启资源管理器...\n");

    // 步骤1：终止所有现有的 explorer.exe 进程
    if (!TerminateAllExplorers()) 
    {
        printf("终止 explorer 失败！错误码: %lu\n", GetLastError());
        return 1;
    }

    // 步骤2：启动新的 explorer.exe
    if (!StartExplorer()) 
    {
        printf("启动 explorer 失败！错误码: %lu\n", GetLastError());
        return 1;
    }

    printf("资源管理器重启成功！\n");

    return 0;
}