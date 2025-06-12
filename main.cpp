#include <windows.h>
#include <shlwapi.h>
#include <iostream>

#include "injector/Injector.h"



/**
 * Program Name
 */
#define PROGRAM "Browser.exe"
/**
 * Module Name
 */
#define MODULE "libhack.dll"



int main() {
    system("cls");

    CHAR directory[MAX_PATH];
    CHAR modulePath[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, directory); // Returns length but that is not needed
    PathCombineA(modulePath, directory, MODULE);

    std::cout << "Full path to module: \033[38;5;10m" << modulePath << "\033[0m" << std::endl;


    // Gets Process PID
    DWORD pid = GetProcessPID(PROGRAM);
    if(pid == 0) {
        std::cerr << "Process not found, make sure it is running."  << std::endl;
        return 1;
    }
    std::cout << "Found process \033[38;5;3mid=0x" << std::hex << pid << "\033[0m" << std::endl;


    // Injects Hack into Browser Process
    std::cout << "\033[38;5;12mInjecting...\033[0m" << std::endl;
    if(!InjectModule(pid, modulePath)) {
        std::cerr << "Injection, Failed!" << std::endl;
        return 1;
    }
    std::cout << "Injection, \033[38;5;10mSuccess!\033[0m" << std::endl;
    return 0;
}
