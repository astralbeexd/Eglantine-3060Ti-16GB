#include <windows.h>
#include <iostream>
#include <string>
#include "common/thread.h"
int main(){
    Common::SetCurrentThreadName("Eglantine-ThreadProbe");
    PWSTR name=nullptr;
    auto result=GetThreadDescription(GetCurrentThread(),&name);
    bool ok=result>=0 && name && std::wstring{name}==L"Eglantine-ThreadProbe";
    if(name)LocalFree(name);
    std::cout << (ok?"PASS: Windows thread description visible from MinGW\n":"FAIL: thread description missing\n");
    return ok?0:2;
}
