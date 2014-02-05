#include "stdafx.h"
#include "PathOfExile.h"

#define POE_PROCESS_NAME _T("PathOfExile.exe")
#define POE_PROCESS_NAME_STEAM _T("PathOfExileSteam.exe")

#define BASE_MGR_PTR_SIG "\x53\x55\x56\x57\x33\xFF\x3B\xC7"
#define BASE_MGR_PTR_MASK "xxxxxxxx"
#define BASE_MGR_PTR_OFFSET 0x0A


CPathOfExile::CPathOfExile() : m_dwModuleAddress(0), m_dwModuleSize(0), 
	m_dwBaseManager(0), m_dwHeroManager(0), m_bIsSteamProcess(false)
{

}


CPathOfExile::~CPathOfExile()
{

}


BOOL CPathOfExile::Attach()
{
	m_bIsSteamProcess = false;
	DWORD dwProcessId = CProcess::Find(POE_PROCESS_NAME);
	if ( dwProcessId == 0 )
	{
		dwProcessId = CProcess::Find(POE_PROCESS_NAME_STEAM);
		if ( dwProcessId == 0 )
		{
			return FALSE;
		}

		m_bIsSteamProcess = true;
	}

	if ( m_bIsSteamProcess && !CProcess::GetModuleInfo(POE_PROCESS_NAME_STEAM, dwProcessId, &m_dwModuleAddress, &m_dwModuleSize) )
	{
		return FALSE;
	}
	else if ( !m_bIsSteamProcess && !CProcess::GetModuleInfo(POE_PROCESS_NAME, dwProcessId, &m_dwModuleAddress, &m_dwModuleSize) )
	{
		return FALSE;
	}

	if ( Open(dwProcessId) )
	{
		m_dwBaseManager = FindPattern(m_dwModuleAddress, m_dwModuleSize, (BYTE*)BASE_MGR_PTR_SIG, BASE_MGR_PTR_MASK);
		if ( m_dwBaseManager )
		{
			m_dwBaseManager -= BASE_MGR_PTR_OFFSET;

			int arrHeroBaseOffsets[5] = { 0x0, 0x4, 0x7C, 0x94, 0xBC };
			if ( m_bIsSteamProcess )
			{
				arrHeroBaseOffsets[4] = 0xD8;
			}

			m_dwHeroManager = GetMultiLevelPointer(m_dwBaseManager, arrHeroBaseOffsets, 5);
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CPathOfExile::IsInGame()
{
	DWORD dwHeroPtr = 0;
	if ( m_dwHeroManager && ReadMemory(m_dwHeroManager, (LPVOID)&dwHeroPtr, sizeof(DWORD)) )
	{
		return dwHeroPtr != 0 ? TRUE : FALSE;
	}

	return FALSE;
}


BOOL CPathOfExile::GetPlayerStats(PLAYERSTATS* stats)
{
	DWORD dwStatsPtr = 0;
	int arrPlayerStatsOffsets[4] = { 0x59C, 0x4, 0xC, 0x50 };
	
	if ( m_dwHeroManager && ((dwStatsPtr = GetMultiLevelPointer(m_dwHeroManager, arrPlayerStatsOffsets, 4)) != 0) )
	{
		if ( ReadMemory(dwStatsPtr, (LPVOID)stats, sizeof(PLAYERSTATS)) )
		{
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CPathOfExile::GetPlayerExperience(PLAYEREXPERIENCE* exp)
{
	DWORD dwExpPtr = 0;
	int arrPlayerExpOffsets[2] = { 0x24, 0x97c };
	
	if ( m_dwHeroManager && ((dwExpPtr = GetMultiLevelPointer(m_dwHeroManager, arrPlayerExpOffsets, 2)) != 0) )
	{
		if ( ReadMemory(dwExpPtr, (LPVOID)exp, sizeof(PLAYEREXPERIENCE)) )
		{
			return TRUE;
		}
	}

	return FALSE;
}