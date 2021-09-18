#pragma once

#include <Windows.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

wchar_t* get_vbox_path();
wchar_t* get_inject_dll();