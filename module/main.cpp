#include "main.h"

#include <iostream>
#include <cstdint>
#include <algorithm>

#include "browser/DocumentManager.h"



void hook();



DWORD WINAPI Main(LPVOID param) {
    std::cout << "\033[38;5;9m -> Attached to Browser Process.\033[0m" << std::endl;
    uintptr_t browser = reinterpret_cast<uintptr_t>(GetModuleHandleA("Browser.exe"));
    DocumentManager* docsManager = reinterpret_cast<DocumentManager*>(browser + 0x121A0); // 0x121A0



    const char* document = "# Test Loading Document";
    docsManager->loadDocument(document);
    std::cout << "Loaded -> \033[38;5;3m" << document << "\033[0m" << std::endl;



    hook(); // Installs Hook



    while(true) { // Cheat Function Loop
        Sleep(2);
    }
}





void* GetAbsoluteFromOffset32(char const *moduleName, unsigned int offset) {
    DWORD64 modAddr = (DWORD64)GetModuleHandleA(moduleName);
    if (modAddr == 0) return nullptr;
    return (void*)(modAddr + offset);
}

void* AllocatePageNearAddress(void* targetAddr) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    const uint64_t PAGE_SIZE = sysInfo.dwPageSize;

    uint64_t startAddr = (uint64_t(targetAddr) & ~(PAGE_SIZE - 1)); // Round down to nearest page boundary
    uint64_t minAddr = std::min(startAddr - 0x7FFFFF00, (uint64_t)sysInfo.lpMinimumApplicationAddress);
    uint64_t maxAddr = std::max(startAddr + 0x7FFFFF00, (uint64_t)sysInfo.lpMaximumApplicationAddress);

    uint64_t startPage = (startAddr - (startAddr % PAGE_SIZE));

    uint64_t pageOffset = 1;
    while (1) {
        uint64_t byteOffset = pageOffset * PAGE_SIZE;
        uint64_t highAddr = startPage + byteOffset;
		    uint64_t lowAddr = (startPage > byteOffset) ? startPage - byteOffset : 0;

        bool needsExit = highAddr > maxAddr && lowAddr < minAddr;

        if(highAddr < maxAddr) {
            void* outAddr = VirtualAlloc((void*)highAddr, PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (outAddr)
                return outAddr;
        }

        if(lowAddr > minAddr) {
            void* outAddr = VirtualAlloc((void*)lowAddr, PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (outAddr != nullptr)
                return outAddr;
        }

        pageOffset++;

        if(needsExit) {
            break;
        }
    }

    return nullptr;
}



void func() {
    MessageBox(nullptr, "Loaded Document", "DocsManager", MB_OK);
}



void* ptr = (void*)&func;
// void* ret_ptr = nullptr;

void hook() {
    // void* interpolate_address = VirtualAlloc(nullptr, 100, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    // void* function_to_hook = GetAbsoluteFromOffset32("Browser.exe", 0x1F64);


    void* function_to_hook = (void*)&UpdateWindow;
    void* tranpoline_address = AllocatePageNearAddress(function_to_hook);

    uint8_t instructions[] = { // Instructions -> mov r10, addr      ;       jmp r10
        0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x41, 0xFF, 0xE2
    };
    uint64_t jmpToFunction = reinterpret_cast<uint64_t>(ptr); // Gets function address and writes to address
    memcpy(&instructions[2], &jmpToFunction, sizeof(jmpToFunction));
    memcpy(tranpoline_address, instructions, sizeof(instructions));



    // uintptr_t jump = static_cast<uintptr_t>(address - (function + 5));

    // memcpy(interpolate_address, function_to_hook, 15);
    // ret_ptr = &function_to_hook;
    //
    //
    // // Instructions: mov rax, &func   ;   jmp rax
    // BYTE mov_rax[] = { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    // BYTE jmp_rax[] = { 0xFF, 0xE0 };
    //
    // Instructions ( Hook ): call address   ;   jmp return address
    // BYTE call_int[] = { 0xFF, 0x15, 0x00, 0x00, 0x00, 0x00 };
    // BYTE jmp_ret[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
    //
    //
    //
    // *(DWORD*)&call_int[2] = ((DWORD64)&ptr - (DWORD64)&tranpoline_address);
    // *(DWORD*)&jmp_ret[2] = ((DWORD64)&ret_ptr - (DWORD64)&tranpoline_address);
    //
    // *(DWORD64*)&mov_rax[2] = (DWORD64)interpolate_address;
    //
    // memcpy(&tranpoline_address[(sizeof(instructions) + 6)], call_int, 6);
    // memcpy(&tranpoline_address[(sizeof(instructions) + 12)], jmp_ret, 6);



    DWORD dwOldProtect;

    VirtualProtect(function_to_hook, 1024, PAGE_EXECUTE_READWRITE, &dwOldProtect);

    uint8_t jmpInstruction[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
    const uint64_t offset = reinterpret_cast<uint64_t>(tranpoline_address) - (reinterpret_cast<uint64_t>(function_to_hook) + sizeof(jmpInstruction));
    memcpy(jmpInstruction + 1, &offset, 4);
    memcpy(function_to_hook, jmpInstruction, sizeof(jmpInstruction));

    // memcpy(&function_to_hook[0], mov_rax, 10);
    // memcpy(&function_to_hook[10], jmp_rax, 2);

    VirtualProtect(function_to_hook, 1024, dwOldProtect, nullptr);
}





// void WriteAbsoluteJump64(void* absJumpMemory, void* addrToJumpTo)
// {
//     uint8_t absJumpInstructions[] =
//     {
//       0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //mov r10, addr
//       0x41, 0xFF, 0xE2 //jmp r10
//     };
//
//     uint64_t addrToJumpTo64 = (uint64_t)addrToJumpTo;
//     memcpy(&absJumpInstructions[2], &addrToJumpTo64, sizeof(addrToJumpTo64));
//     memcpy(absJumpMemory, absJumpInstructions, sizeof(absJumpInstructions));
// }



// void InstallHook(void* func2hook, void* payloadFunction)
// {
//     void* relayFuncMemory = AllocatePageNearAddress(func2hook);
//     WriteAbsoluteJump64(relayFuncMemory, payloadFunction); //write relay func instructions
//
//     //now that the relay function is built, we need to install the E9 jump into the target func,
//     //this will jump to the relay function
//     DWORD oldProtect;
//     VirtualProtect(func2hook, 1024, PAGE_EXECUTE_READWRITE, &oldProtect);
//
//     //32 bit relative jump opcode is E9, takes 1 32 bit operand for jump offset
//     uint8_t jmpInstruction[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
//
//     //to fill out the last 4 bytes of jmpInstruction, we need the offset between
//     //the relay function and the instruction immediately AFTER the jmp instruction
//     const uint64_t relAddr = (uint64_t)relayFuncMemory - ((uint64_t)func2hook + sizeof(jmpInstruction));
//     memcpy(jmpInstruction + 1, &relAddr, 4);
//
//     //install the hook
//     memcpy(func2hook, jmpInstruction, sizeof(jmpInstruction));
// }
