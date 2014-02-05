#pragma once

#define PROCESS_DEFAULT_ACCESS (PROCESS_VM_READ | PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION)

class CProcess
{
protected:
	HANDLE m_hProcess;

public:
	CProcess();
	virtual ~CProcess();

	BOOL Open(DWORD dwProcessId, DWORD dwDesiredAccess = PROCESS_DEFAULT_ACCESS, BOOL bInheritHandle = FALSE);

	BOOL ReadMemory(DWORD dwBaseAddress, LPVOID lpBuffer, DWORD dwSize);

	BOOL IsRunning();
	BOOL Terminate(UINT uExitCode = EXIT_SUCCESS);

	DWORD GetMultiLevelPointer(DWORD dwStartAddress, int arrOffsets[], int nOffsets);

	DWORD FindPattern(DWORD dwStartAddress, DWORD dwSize, BYTE* bMask, char* szMask);
	bool IsPatternMatch(BYTE* pData, BYTE* bMask, char* szMask);

	static DWORD Find(PCTSTR pszProcessName);
	static BOOL GetModuleInfo(PCTSTR pszProcessName, DWORD dwProcessId, LPDWORD pdwBaseAddress, LPDWORD pdwBaseSize);
};