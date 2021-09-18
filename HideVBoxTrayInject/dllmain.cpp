#include <Windows.h>

#include "MinHook.h"

#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif

typedef BOOL (WINAPI* SHELL_NOTIFYICONW)(DWORD, PNOTIFYICONDATAW);

SHELL_NOTIFYICONW fpShell_NotifyIconW = NULL;

BOOL WINAPI DetourShell_NotifyIconW(DWORD dwMessage, PNOTIFYICONDATAW lpData) {
	return TRUE;
}

typedef BOOL(WINAPI* SHELL_NOTIFYICONA)(DWORD, PNOTIFYICONDATAA);

SHELL_NOTIFYICONA fpShell_NotifyIconA = NULL;

BOOL WINAPI DetourShell_NotifyIconA(DWORD dwMessage, PNOTIFYICONDATAA lpData) {
    return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
            if (MH_Initialize() != MH_OK)
            {
                return 1;
            }

            if (MH_CreateHook(&Shell_NotifyIconW, &DetourShell_NotifyIconW, reinterpret_cast<LPVOID*>(&fpShell_NotifyIconW)) != MH_OK)
            {
                return 1;
            }

            if (MH_CreateHook(&Shell_NotifyIconA, &DetourShell_NotifyIconA, reinterpret_cast<LPVOID*>(&fpShell_NotifyIconA)) != MH_OK)
            {
                return 1;
            }

            if (MH_EnableHook(&Shell_NotifyIconW) != MH_OK)
            {
                return 1;
            }

            if (MH_EnableHook(&Shell_NotifyIconA) != MH_OK)
            {
                return 1;
            }

			break;
		}

		case DLL_THREAD_ATTACH:
		{
			break;
		}

		case DLL_THREAD_DETACH:
		{
			break;
		}

		case DLL_PROCESS_DETACH:
		{
            // Disable the hook for MessageBoxW.
            if (MH_DisableHook(&Shell_NotifyIconW) != MH_OK)
            {
                return 1;
            }

            if (MH_DisableHook(&Shell_NotifyIconA) != MH_OK)
            {
                return 1;
            }
            
            // Uninitialize MinHook.
            if (MH_Uninitialize() != MH_OK)
            {
                return 1;
            }
            
			break;
		}
        
	}
	return TRUE;
}