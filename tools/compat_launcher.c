/* SPDX-License-Identifier: GPL-3.0-or-later
 * Compatibility launcher prepared with AI assistance. */
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cpuid.h>
#include <stdio.h>
#include <wchar.h>

static int supports_v2(unsigned basic_ecx, unsigned extended_ecx) {
    const unsigned required = (1u << 0) | (1u << 9) | (1u << 13) |
                              (1u << 19) | (1u << 20) | (1u << 23);
    return (basic_ecx & required) == required && (extended_ecx & 1u);
}

#ifdef COMPAT_TEST
int main(void) {
    const unsigned bits[] = {0,9,13,19,20,23};
    unsigned mask=0;
    for (unsigned i=0;i<6;++i) mask |= 1u << bits[i];
    if (!supports_v2(mask,1) || supports_v2(mask,0)) return 1;
    for (unsigned i=0;i<6;++i) if (supports_v2(mask & ~(1u << bits[i]),1)) return 2;
    puts("PASS: x86-64-v2 required-feature checks");
    return 0;
}
#else
static void explain(const wchar_t* text) {
    MessageBoxW(NULL,text,L"Eglantine - compatibilidad",MB_OK | MB_ICONERROR);
}

/* Apply the same clean defaults whether opened through the CMD or directly. */
static int clean_environment(void) {
    wchar_t windows[32768], search_path[32768];
    UINT length=GetWindowsDirectoryW(windows,32768);
    if (!length || length>=32768 ||
        swprintf(search_path,32768,L"%ls\\System32;%ls",windows,windows)<0) return 0;
    LPWCH variables=GetEnvironmentStringsW();
    if (!variables) return 0;
    int ok=1;
    for (const wchar_t* entry=variables; *entry; entry+=wcslen(entry)+1) {
        if (_wcsnicmp(entry,L"EDEN_NX2_",9)!=0) continue;
        const wchar_t* equals=wcschr(entry,L'=');
        if (!equals || equals-entry>=32768) continue;
        wchar_t name[32768];
        wmemcpy(name,entry,(size_t)(equals-entry));name[equals-entry]=L'\0';
        if (!SetEnvironmentVariableW(name,NULL)) ok=0;
    }
    FreeEnvironmentStringsW(variables);
    return SetEnvironmentVariableW(L"PATH",search_path) &&
           SetEnvironmentVariableW(L"QT_PLUGIN_PATH",NULL) &&
           SetEnvironmentVariableW(L"QT_QPA_PLATFORM_PLUGIN_PATH",NULL) &&
           SetEnvironmentVariableW(L"EDEN_NX2_SMALL_VRAM_PROFILE",L"0") && ok;
}

int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR arguments,int show) {
    (void)instance; (void)previous; (void)show;
    unsigned a=0,b=0,c=0,d=0,base=0,extended=0;
    if (__get_cpuid(1,&a,&b,&c,&d)) base=c;
    if (__get_cpuid(0x80000001u,&a,&b,&c,&d)) extended=c;
    OSVERSIONINFOW version={0};version.dwOSVersionInfoSize=sizeof(version);
    typedef LONG (WINAPI *VersionFunction)(OSVERSIONINFOW*);
    VersionFunction query=(VersionFunction)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"RtlGetVersion");
    if (!query || query(&version)!=0) {
        explain(L"No se pudo comprobar la version de Windows."); return 2;
    }
    const int cpu_ok=supports_v2(base,extended);
    const int os_ok=version.dwMajorVersion>=10 && version.dwBuildNumber>=17763;
    if (wcscmp(arguments,L"--check")==0) {
        printf("cpu_x86_64_v2=%d windows_build=%lu os_target_ok=%d\n",cpu_ok,version.dwBuildNumber,os_ok);
        return cpu_ok && os_ok ? 0 : 3;
    }
    if (!os_ok) {
        explain(L"Este paquete requiere Windows de 64 bits, version 10 (1809 o posterior) u 11."); return 3;
    }
    if (!cpu_ok) {
        explain(L"El procesador no cumple el nivel x86-64-v2 requerido por este paquete. No se ha iniciado el emulador."); return 4;
    }
    wchar_t exe[32768],directory[32768];
    DWORD length=GetModuleFileNameW(NULL,directory,32768);
    if (!length || length>=32768) { explain(L"La ruta de instalacion es demasiado larga."); return 5; }
    wchar_t* slash=wcsrchr(directory,L'\\');
    if (!slash) return 5;
    *slash=L'\0';
    if (swprintf(exe,32768,L"%ls\\eglantine.exe",directory)<0) return 5;
    /* Preserve the raw Windows argument quoting; never invoke a shell. */
    size_t capacity=wcslen(exe)+wcslen(arguments)+4;
    if (capacity>32767) { explain(L"Los argumentos son demasiado largos."); return 5; }
    wchar_t* command=HeapAlloc(GetProcessHeap(),0,capacity*sizeof(wchar_t));
    if (!command) { explain(L"No hay memoria para iniciar el emulador."); return 5; }
    if (swprintf(command,capacity,L"\"%ls\" %ls",exe,arguments)<0 || !clean_environment()) {
        HeapFree(GetProcessHeap(),0,command);
        explain(L"No se pudo preparar el entorno del emulador."); return 5;
    }
    STARTUPINFOW startup={0};startup.cb=sizeof(startup);
    PROCESS_INFORMATION process={0};
    if (!CreateProcessW(exe,command,NULL,NULL,FALSE,0,NULL,directory,&startup,&process)) {
        DWORD error=GetLastError();
        HeapFree(GetProcessHeap(),0,command);
        wchar_t message[512];
        swprintf(message,512,L"No se pudo abrir eglantine.exe (error Windows %lu). Extrae el ZIP completo y conserva juntos el programa y sus DLL.",error);
        explain(message);return 6;
    }
    HeapFree(GetProcessHeap(),0,command);
    CloseHandle(process.hThread);
    WaitForSingleObject(process.hProcess,INFINITE);
    DWORD result=0;
    if (!GetExitCodeProcess(process.hProcess,&result)) result=GetLastError();
    CloseHandle(process.hProcess);
    if (result) {
        wchar_t message[512];
        swprintf(message,512,L"El emulador termino con codigo 0x%08lX. 0xC000001D indica una instruccion no admitida; 0xC0000135, una dependencia ausente. Comparte este codigo y la version del paquete al informar del fallo.",result);
        explain(message);
    }
    return (int)result;
}
#endif
