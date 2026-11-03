/* SPDX-License-Identifier: GPL-3.0-or-later
 * AI-assisted integration fixture. No emulator or game is executed. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
int wmain(int argc,wchar_t** argv) {
    wchar_t value[32768],windows[32768],expected[32768];
    if (argc!=3 || wcscmp(argv[1],L"archivo con espacios \u00e1.nsp") ||
        wcscmp(argv[2],L"--prueba")) return 10;
    if (GetEnvironmentVariableW(L"QT_PLUGIN_PATH",value,32768) ||
        GetEnvironmentVariableW(L"QT_QPA_PLATFORM_PLUGIN_PATH",value,32768) ||
        GetEnvironmentVariableW(L"EDEN_NX2_TEST_UNKNOWN",value,32768)) return 11;
    if (!GetEnvironmentVariableW(L"EDEN_NX2_SMALL_VRAM_PROFILE",value,32768) || wcscmp(value,L"0")) return 12;
    if (!GetWindowsDirectoryW(windows,32768) ||
        swprintf(expected,32768,L"%ls\\System32;%ls",windows,windows)<0 ||
        !GetEnvironmentVariableW(L"PATH",value,32768) || wcscmp(value,expected)) return 13;
    FILE* out=fopen("launcher-child-ok.txt","wb");
    if (!out) return 14;
    fputs("PASS: arguments, Unicode, spaces, working directory and clean environment\n",out);
    return fclose(out)!=0;
}
