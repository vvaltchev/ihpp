#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <strsafe.h>

void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    //MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

	printf("Error: %s\n", lpDisplayBuf);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}


int main(int argc, char ** argv) {

	DWORD tick1,tick2;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
	char *cmdline=0;
	int i;
	bool res;
	size_t cmdline_size=1;

    ZeroMemory( &si, sizeof(si) );
    ZeroMemory( &pi, sizeof(pi) );

	si.cb = sizeof(si);
	
	if (argc < 2) {
	
		printf("syntax: wintime <command> [command arg1] [command arg2...]\n");
		return 0;
	}

	for (i=1; i < argc; i++)
		cmdline_size+=strlen(argv[i])+1;

	cmdline = (char*) malloc(cmdline_size);

	memset(cmdline,0,cmdline_size);

	strcat(cmdline, argv[1]);

	for (i=2; i < argc; i++) {
		strcat(cmdline, " ");
		strcat(cmdline, argv[i]);
	}

	printf("Command line: '%s'\n", cmdline);

	tick1 = GetTickCount();

	res = CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	if (!res) {
	
		ErrorExit(TEXT("main"));
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	tick2 = GetTickCount();

	printf("Time elapsed: %f sec\n", (tick2-tick1)/1000.0);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}