#include "stdafx.h"
#include "Process.h"

#include <psapi.h>
#include <tlhelp32.h>
#pragma comment(lib, "psapi.lib")

#define PAGE_SIZE 8192


CProcess::CProcess() : m_hProcess(NULL)
{

}


CProcess::~CProcess()
{
	if ( m_hProcess != NULL )
	{
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
	}
}


BOOL CProcess::Open(DWORD dwProcessId, DWORD dwDesiredAccess, BOOL bInheritHandle)
{
	m_hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	return (m_hProcess != NULL) ? TRUE : FALSE;
}


BOOL CProcess::ReadMemory(DWORD dwBaseAddress, LPVOID lpBuffer, DWORD dwSize)
{
	DWORD dwBytesRead = 0;
	BOOL bResult = ReadProcessMemory(m_hProcess, (LPCVOID)dwBaseAddress, lpBuffer, dwSize, &dwBytesRead);
	return (bResult && dwSize == dwBytesRead) ? TRUE : FALSE;
}


BOOL CProcess::IsRunning()
{
	DWORD dwExitCode = 0;
	BOOL bResult = GetExitCodeProcess(m_hProcess, &dwExitCode);
	return (bResult && dwExitCode == STILL_ACTIVE) ? TRUE : FALSE;
}


BOOL CProcess::Terminate(UINT uExitCode)
{
	BOOL bResult = TerminateProcess(m_hProcess, uExitCode);
	m_hProcess = NULL;
	return bResult;
}


DWORD CProcess::GetMultiLevelPointer(DWORD dwStartAddress, int arrOffsets[], int nOffsets)
{
	DWORD dwValue = 0;
	DWORD dwAddress = dwStartAddress;
	BOOL bRes = ReadMemory(dwAddress, (LPVOID)&dwValue, sizeof(dwValue));
	if ( bRes )
	{
		for ( int i = 0 ; i < nOffsets; i++ )
		{
			dwAddress = dwValue + arrOffsets[i];
			bRes = ReadMemory(dwAddress, (LPVOID)&dwValue, sizeof(dwValue));
			if ( !bRes )
			{
				return 0;
			}
		}

		return dwAddress;
	}

	return 0;
}


DWORD CProcess::FindPattern(DWORD dwStartAddress, DWORD dwSize, BYTE* bMask, char* szMask)
{
	DWORD dwBytesRead = 0;
	DWORD dwCurrentOffset = 0;
	DWORD dwCurrentAddress = dwStartAddress;

	while ( dwCurrentOffset < dwSize )
	{
		BYTE bMemoryPage[PAGE_SIZE];
		BOOL bRes = ReadProcessMemory(m_hProcess, (void*)dwCurrentAddress, (void*)&bMemoryPage, PAGE_SIZE, &dwBytesRead);
		if ( bRes )
		{
			for ( DWORD dwIndex = 0; dwIndex < PAGE_SIZE; dwIndex++ )
			{
				if ( IsPatternMatch(&bMemoryPage[dwIndex], bMask, szMask) )
				{
					return dwIndex + dwCurrentAddress;
				}
			}
		}

		dwCurrentOffset += PAGE_SIZE;
		dwCurrentAddress += PAGE_SIZE;
	}
	
	return 0;
}


bool CProcess::IsPatternMatch(BYTE* pData, BYTE* bMask, char* szMask)
{
    for ( ; *szMask; ++szMask, ++pData, ++bMask )
	{
        if ( *szMask == 'x' && *pData != *bMask )
		{
            return false;
		}
	}

    return (*szMask) == 0;
}


DWORD CProcess::Find(PCTSTR pszProcessName)
{
	HANDLE h32Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if ( h32Snap != INVALID_HANDLE_VALUE )
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		BOOL bRet = Process32First(h32Snap, &pe32);
		while ( bRet )
		{
			int nProcLen = lstrlen(pe32.szExeFile);
			int nInNameLen = lstrlen(pszProcessName);
			if ( CSTR_EQUAL == CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, pe32.szExeFile, nProcLen, pszProcessName, nInNameLen) )
			{
				CloseHandle(h32Snap);
				return pe32.th32ProcessID;
			}

			bRet = Process32Next(h32Snap, &pe32);
		}

		CloseHandle(h32Snap);
	}

	return 0;
}


BOOL CProcess::GetModuleInfo(PCTSTR pszProcessName, DWORD dwProcessId, LPDWORD pdwBaseAddress, LPDWORD pdwBaseSize)
{
	*pdwBaseSize = 0;
	*pdwBaseAddress = 0;

	BOOL bSuccess = FALSE;
	HANDLE h32Snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if ( h32Snap != INVALID_HANDLE_VALUE )
	{
		MODULEENTRY32 me32;
		me32.dwSize = sizeof(MODULEENTRY32);

		BOOL bRet = Module32First(h32Snap, &me32);
		while ( bRet )
		{
			int nModuleLen = lstrlen(me32.szModule);
			int nInNameLen = lstrlen(pszProcessName);
			if ( CSTR_EQUAL == CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, me32.szModule, nModuleLen, pszProcessName, nInNameLen) )
			{
				bSuccess = TRUE;
				*pdwBaseSize = me32.modBaseSize;
				*pdwBaseAddress = (DWORD)me32.modBaseAddr;

				break;
			}

			bRet = Module32Next(h32Snap, &me32);
		}

		CloseHandle(h32Snap);
	}

	return bSuccess;
}