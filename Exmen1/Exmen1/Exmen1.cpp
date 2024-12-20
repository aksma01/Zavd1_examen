#include <iostream>
#include <Windows.h>
#include <string>

int main() {

    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
        std::cerr << "CreatePipe failed: " << GetLastError() << std::endl;
        return 1;
    }

    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);  
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE); 
    si.hStdInput = hReadPipe;         
    si.dwFlags |= STARTF_USESTDHANDLES;

    std::wstring commandLine = L"cmd.exe";

    if (!CreateProcess(NULL,
        &commandLine[0],
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi)) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        return 1;
    }

    std::string message = "Hello from parent process!\n";
    DWORD dwWritten;
    if (!WriteFile(hWritePipe, message.c_str(), message.length(), &dwWritten, NULL)) {
        std::cerr << "WriteFile failed: " << GetLastError() << std::endl;
        return 1;
    }
    CloseHandle(hWritePipe);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hReadPipe);

    return 0;
}