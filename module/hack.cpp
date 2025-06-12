#include "main.h"
#include "base/pch.h"



BOOL APIENTRY DllMain(HMODULE module, DWORD callReason, LPVOID reserved) {
    switch(callReason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(nullptr, NULL, Main, nullptr, 0, nullptr);

        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }
    return TRUE;
}
