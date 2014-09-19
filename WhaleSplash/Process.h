#pragma once

#define PROCESS_DEFAULT_ACCESS (PROCESS_VM_READ | PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION)

class CProcess
{
protected:
	HANDLE m_hProcess;

public:
	CProcess();
	virtual ~CProcess();

	bool Open(DWORD dwProcessId, DWORD dwDesiredAccess = PROCESS_DEFAULT_ACCESS, BOOL bInheritHandle = FALSE);

	bool ReadMemory(DWORD dwBaseAddress, LPVOID lpBuffer, DWORD dwSize);
	bool WriteMemory(DWORD dwBaseAddress, LPCVOID lpBuffer, DWORD dwSize);

	BYTE ReadByte(DWORD dwBaseAddress);
	void WriteByte(DWORD dwBaseAddress, BYTE bValue);

	DWORD ReadDWORD(DWORD dwBaseAddress);

	bool IsRunning();
	bool Terminate(UINT uExitCode = EXIT_SUCCESS);

	template<size_t N>
	DWORD GetMultiLevelPointer32(DWORD dwStartAddress, DWORD (&arOffsets)[N])
	{
		DWORD address = ReadDWORD(dwStartAddress);
		for ( int i = 0; i < N; i++ )
		{
			address = ReadDWORD(address + arOffsets[i]);
		}

		return address;
	}

	DWORD FindPattern(DWORD dwStartAddress, DWORD dwSize, BYTE* bMask, const char* szMask);
	bool IsPatternMatch(BYTE* pData, BYTE* bMask, const char* szMask);

	static DWORD Find(PCTSTR pszProcessName);
	static BOOL GetModuleInfo(PCTSTR pszProcessName, DWORD dwProcessId, LPDWORD pdwBaseAddress, LPDWORD pdwBaseSize);
};