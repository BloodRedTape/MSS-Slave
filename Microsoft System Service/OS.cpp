#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include "OS.h"
#include <CommCtrl.h>


extern void Main();

#ifdef NDEBUG
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
    Main();
}

void OS::Shutdown(){
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tkp = { 0 };
    bool bRet = false;

    // Get a token for this process. 
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        if (LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
            tkp.PrivilegeCount = 1;  // one privilege to set    
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            // Get the shutdown privilege for this process. 
            if (AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0)) {
                ::CloseHandle(hToken);

                if (ERROR_SUCCESS == GetLastError()) {

                    DWORD dwFlags = EWX_POWEROFF;
                    DWORD dwReason = SHTDN_REASON_MAJOR_SYSTEM;

                    if (ExitWindowsEx(dwFlags, dwReason)) {
                        bRet = true;
                    }
                }
            }
        }
    }
}

void OS::GetPCName(char* buffer, unsigned long* read_size){
    GetComputerNameA(buffer, read_size);
}

uint64_t OS::GetBootupTime(){
    return GetTickCount64();
}

void OS::ExportInExecutable(const char *path, const void* data, size_t size) {
    FILE* file;
    fopen_s(&file, path, "wb");
    if (file) {
        fwrite(data, 1, size, file);

        fclose(file);
    }
    else {
        std::cout << "OS: can't open file for writing\n";
    }
}

void OS::CreateMessageBox(const char* title, const char* text) {
    MessageBoxA(NULL, text, title, MB_OK | MB_ICONERROR);
}

std::string OS::GetCurrentProcessExecutablePath() {
    char szFileName[MAX_PATH];
    auto size = GetModuleFileNameA(NULL, szFileName, MAX_PATH);
    return { szFileName, size };
}

void OS::NewProcess(char* path) {
    STARTUPINFOA info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;

    CreateProcessA(NULL, (char*)path, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
}

void OS::Exit() {
    exit(0);
}

void OS::Sleep(unsigned long ms) {
    ::Sleep(ms);
}

bool OS::Delete(const char* filepath) {
    return DeleteFileA(filepath);
}

bool OS::Move(const char* source, const char* destination) {
    return MoveFileA(source, destination);
}
