#include "Injector.h"

#include <iostream>
#include <tlhelp32.h>



DWORD GetProcessPID(const char* process) {
    PROCESSENTRY32 entry;
    DWORD pid = 0;

    // Creates Snapshot
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(snapshot == INVALID_HANDLE_VALUE) return 0;
    entry.dwSize = sizeof(PROCESSENTRY32);

    bool result = Process32First(snapshot, &entry);
    while(result) {
        if(strcmp(process, entry.szExeFile) == 0) {
            pid = entry.th32ProcessID;
            break;
        }
        result = Process32Next(snapshot, &entry);
    }

    CloseHandle(snapshot); // Cleanup before return
    return pid;
}

HANDLE OpenTargetProcess(const DWORD pid) {
    const HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if(process == nullptr) std::cout << "Opened Process: \033[38;5;9mFailed!\033[0m" << std::endl;
    else std::cout << "Opened Process: \033[38;5;10mSuccess!\033[0m" << std::endl;
    return process;
}

LPVOID GetLoadLibraryAddress() {
    HMODULE module = GetModuleHandleW(L"kernel32.dll");
    if(module == nullptr) return nullptr; // Should Never Happen
    return reinterpret_cast<LPVOID>(GetProcAddress(module, "LoadLibraryA"));
}



bool InjectModule(const DWORD pid, LPCSTR module) {
    HANDLE process = OpenTargetProcess(pid);
    if(process == nullptr) {
        return false;
    }


    // Module Injection Setup
    DWORD pathLength = strlen(module);
    LPVOID remoteBuffer = VirtualAllocEx(
        process, nullptr,
        pathLength + 1,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
    );
    DWORD bytesWritten;
    LPVOID loadLibrary;
    HANDLE thread;

    if(remoteBuffer == nullptr) { // Remote Buffer
        std::cout << "\033[38;5;9mFailed to allocate buffer.\033[0m" << std::endl;
        CloseHandle(process);
        return false;
    }
    std::cout << "Allocated buffer at \033[38;5;3m" << std::hex << remoteBuffer << "\033[0m" << std::endl;

    // Writes Library into Process Memory
    if(!WriteProcessMemory(process, remoteBuffer, module, pathLength, reinterpret_cast<SIZE_T*>(&bytesWritten))) {
        std::cout << "\033[38;5;9mFailed to write memory in process.\033[0m" << std::endl;
        CloseHandle(process);
        return false;
    }
    std::cout << "Wrote \033[38;5;3m" << bytesWritten << "\033[0m bytes to buffer" << std::endl;
    if(bytesWritten != pathLength) { // Checks if Data writen is correct amount
        std::cout << "\033[38;5;9mFailed to write all bytes.\033[0m" << std::endl;
        CloseHandle(process);
        return false;
    }



    // Gets Load Library
    loadLibrary = GetLoadLibraryAddress();
    std::cout << "LoadLibrary=\033[38;5;3m" << std::hex << loadLibrary << "\033[0m" << std::endl;


    // Creates Remote Thread
    std::cout << "\033[38;5;12mStarting Thread ...\033[0m" << std::endl;
    thread = CreateRemoteThread(
        process, nullptr, 0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibrary),
        remoteBuffer,
        0, nullptr
    );
    if(thread == nullptr) {
        std::cout << "\033[38;5;9mFailed to create thread.\033[0m" << std::endl;
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }


    std::cout << "\033[38;5;12mWating for thread response ...\033[0m" << std::endl;
    WaitForSingleObject(thread, INFINITE);
    std::cout << "\033[38;5;12mResponse: DLLMain success, DLL injected.\033[0m" << std::endl;


    VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
    CloseHandle(process);
    return true;
}
