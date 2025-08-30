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
		printf("无法打开服务控制管理器，错误代码: %lu\n", GetLastError());
		return;
	}

	hService = OpenServiceA(hSCManager,name,SERVICE_START);


	if (!StartService(hService, 0, NULL)) 
	{
		printf("启动服务失败，服务可能已经启动，且仍会启动r星平台，错误代码: %lu\n", GetLastError());
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return;
	}

	printf("服务已成功启动,即将启动r星平台\n");


	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
}

int main()
{
	const char *name = "Rockstar Service";
	StartService(name);							//启动服务
	Sleep(5000);


	// 获取当前用户名
	TCHAR username[256] = { 0 };
	DWORD usernameSize = GetEnvironmentVariable(TEXT("USERNAME"), username, sizeof(username) / sizeof(TCHAR));
	if (usernameSize == 0 || usernameSize >= sizeof(username) / sizeof(TCHAR)) {
		_tprintf(TEXT("获取用户名失败，错误码: %lu\n"), GetLastError());
		return 1;
	}

	// 构建Rockstar Games路径
	TCHAR rockstarDir[MAX_PATH] = { 0 };
	_stprintf_s(rockstarDir, sizeof(rockstarDir) / sizeof(TCHAR),
		TEXT("C:\\Users\\%s\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Rockstar Games"),username);

	// 构建快捷方式完整路径（将"Rockstar Games Launcher.lnk"替换为实际快捷方式名称）
	TCHAR lnkPath[MAX_PATH] = { 0 };
	_stprintf_s(lnkPath, sizeof(lnkPath) / sizeof(TCHAR),
		TEXT("%s\\Rockstar Games Launcher.lnk"), rockstarDir);

	// 启动
	HINSTANCE hInstance = ShellExecute(NULL,TEXT("open"),lnkPath,NULL,rockstarDir,SW_SHOWNORMAL);

	if ((INT_PTR)hInstance <= 32) {
		_tprintf(TEXT("启动失败，错误码: %lu\n"), GetLastError());
		return 1;
	}
	
	return 0;
}