#include <Windows.h>
#include "util.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) 
{
	void* page;
	STARTUPINFO si = { sizeof si };
	PROCESS_INFORMATION pi = { 0 };
	HANDLE hThread;
	
	wchar_t * exe_path = get_vbox_path();
	
	if (!CreateProcess(NULL, exe_path, NULL, NULL, FALSE, CREATE_SUSPENDED | DEBUG_PROCESS, NULL, NULL, &si, &pi)) {
		return 1;
	}
	
	delete exe_path;
	
	wchar_t* lib_path = get_inject_dll();
	
	HMODULE hModule = GetModuleHandle(L"kernel32.dll");
	LPVOID lpBaseAddress = (LPVOID)GetProcAddress(hModule, "LoadLibraryW");
	
	if (lpBaseAddress == NULL)
	{
		return 2;
	}
	
	// Allocate a page in memory for the arguments of LoadLibrary.
	page = VirtualAllocEx(pi.hProcess, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (page == NULL) {
		return 3;
	}
	
	if (GetFileAttributes(lib_path) == INVALID_FILE_ATTRIBUTES) {
		return 4;
	}
	
	// Write library path to the page used for LoadLibrary arguments.
	if (WriteProcessMemory(pi.hProcess, page, lib_path, MAX_PATH, NULL) == 0) {
		return 5;
	}
	
	delete lib_path;

	// Inject the shared library into the address space of the process,
	// through a call to LoadLibrary.
	hThread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpBaseAddress, page, 0, NULL);
	if (hThread == NULL) {
		return 6;
	}

	Sleep(1000);

	// Cleanup.
	CloseHandle(hThread);

	// Resume the execution of the process, once all libraries have been injected
	// into its address space.
	if (ResumeThread(pi.hThread) == -1) {
		return 7;
	}
	
	DebugActiveProcessStop(pi.dwProcessId);

	//WaitForSingleObject(pi.hProcess, INFINITE);

	// Cleanup.
	CloseHandle(pi.hProcess);
	VirtualFreeEx(pi.hProcess, page, MAX_PATH, MEM_RELEASE);

	return 0;
}