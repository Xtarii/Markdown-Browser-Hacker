#ifndef INJECTOR_H
#define INJECTOR_H

#include <windows.h>



/**
 * Gets Process PID
 *
 * @param process Process Name
 * @return Process PID
 */
DWORD GetProcessPID(const char* process);

/**
 * Opens Process
 *
 * @param pid Process ID
 * @return Process Handle
 */
HANDLE OpenTargetProcess(DWORD pid);

/**
 * Gets Load Library Address
 *
 * @return Library Address
 */
LPVOID GetLoadLibraryAddress();


/**
 * Creates remote buffer
 *
 * Returns the Buffer as **out**
 * and returns creation status.
 *
 * @param SIZE Buffer Size
 * @param process Process
 * @param out Output Buffer
 * @return Run Status
 */
bool CreateRemoteBuffer(DWORD SIZE, HANDLE process, LPVOID& out);

/**
 * Writes Module Bytes to Process Memory
 *
 * Outputs Write buffer size and returns
 * the status of the writing.
 *
 * @param process Process
 * @param buffer Buffer
 * @param module Module
 * @param SIZE Size
 * @param out Output
 * @return Write Status
 */
bool WriteBytesToProcess(HANDLE process, LPVOID buffer, LPCSTR module, DWORD SIZE, DWORD& out);

/**
 * Creates Remote Thread
 *
 * Outputs Thread and returns
 * the status of the thread initialization.
 *
 * @param process Process
 * @param loadLibrary Load Library Function
 * @param buffer Buffer
 * @param out Output Thread
 * @return Thread Status
 */
bool CreateThread(HANDLE process, LPVOID loadLibrary, LPVOID buffer, HANDLE& out);





/**
 * Injects Module into Process
 *
 * @param pid Process ID
 * @param module Module Path
 * @return Success Status
 */
bool InjectModule(DWORD pid, LPCSTR module);

#endif //INJECTOR_H
