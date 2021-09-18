#include "util.h"

wchar_t* get_vbox_path() {
	wchar_t system32[MAX_PATH] = { 0 };
	wchar_t * command = new wchar_t[MAX_PATH];
	GetSystemDirectoryW(system32, MAX_PATH);

	lstrcpyW(command, system32);
	lstrcatW(command, L"\\VBoxTray.exe");

	return command;
}

wchar_t* get_inject_dll() {
#if defined _M_X64
	const wchar_t* dll = L"HideVBoxTrayInjectx64.dll";
#elif defined _M_IX86
	const wchar_t* dll = L"HideVBoxTrayInjectx86.dll";
#endif

	HMODULE hExe = GetModuleHandle(NULL);
	wchar_t * fullPath = new wchar_t[MAX_PATH];
	GetModuleFileName(hExe, fullPath, MAX_PATH);
	PathRemoveFileSpec(fullPath);

	lstrcatW(fullPath, L"\\");
	lstrcatW(fullPath, dll);

	return fullPath;
}