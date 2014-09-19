#include "stdafx.h"
#include "PathOfExile.h"

const TCHAR* CPathOfExile::s_ProcessName = _T("PathOfExileSteam.exe");

const DWORD CPathOfExile::s_BaseOffset = 0xA;
const char* CPathOfExile::s_BaseMask = "xxxxxxxx";
const BYTE CPathOfExile::s_BaseSignature[] = {
	0x53, 0x55, 0x56, 0x57, 0x33, 0xff, 0x3b, 0xc7
};

const char* CPathOfExile::s_MapMask = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const BYTE CPathOfExile::s_MapSignature[] = {
    0x51, 0x8b, 70, 0x68, 0x8b, 8, 0x68, 0, 0x20, 0, 0, 0x8d, 0x54, 0x24, 4, 0x52, 
    0x6a, 0, 0x6a, 0, 80, 0x8b, 0x41, 0x2c, 0xff, 0xd0, 0x8b, 70, 0x48, 0x3b, 70, 0x4c
};

const DWORD CPathOfExile::s_ExperienceThresholds[] = { 
	0, 0, 0x20d, 0x6e0, 0xec5, 0x1c10, 0x2f9a, 0x4b7c, 0x72c1, 0xa8ad, 
	0xf0fd, 0x14fe6, 0x1cb02, 0x266c8, 0x32b78, 0x41ead, 0x5495e, 0x6b3e4, 0x8697f, 0xa7473, 
	0xcdfbd, 0xfba4e, 0x13115d, 0x16f6e3, 0x1b7a8f, 0x20b1bc, 0x26aed2, 0x2d8ae5, 0x355b5e, 0x3e440f, 
	0x485eb4, 0x53c647, 0x60a7b9, 0x6f22f6, 0x7f62ee, 0x9195f6, 0xa5edef, 0xbca072, 0xd5e6f1, 0xf1fee8, 
	0x11137e2, 0x133cacf, 0x15a11bf, 0x1846dfb, 0x1b325e1, 0x1e6b8ab, 0x21f8a47, 0x25e1839, 0x2a2d3b5, 0x2ee5ff0, 
	0x3413e6b, 0x39c2757, 0x3ffc6e5, 0x46cedeb, 0x4e45feb, 0x566eb81, 0x5f5a64e, 0x6917b0b, 0x73b8101, 0x7f50172, 
	0x8bf37b2, 0x99b94d0, 0xa8ba004, 0xb90f765, 0xcad5092, 0xde2a327, 0xf330414, 0x10a079d3, 0x122d81cc, 0x13dcbdf0, 
	0x15b0c473, 0x17acb339, 0x19d3d4b1, 0x1c296ad2, 0x1eb1515c, 0x216f996a, 0x24684f76, 0x27a06a60, 0x2b1ca779, 0x2ee2c1b8, 
	0x32f83c28, 0x37636706, 0x3c2ae3d3, 0x41563835, 0x46ecb622, 0x4cf738ce, 0x537e7079, 0x5a8c16d6, 0x622a5cf2, 0x6a649790, 
	0x7346a557, 0x7cdda443, 0x87374fde, 0x92626268, 0x9e6f0170, 0xab6e0fa5, 0xb9725eac, 0xc88f3402, 0xd8da52c6, 0xea6a0c62, 0xfd56f4ec
};


CPathOfExile::CPathOfExile() : m_dwModuleAddress(0), m_dwModuleSize(0), 
	m_dwManagerBase(0), m_bWriteAccess(false)
{

}


CPathOfExile::~CPathOfExile()
{
	
}


bool CPathOfExile::Attach(bool bWriteAccess)
{
	m_dwManagerBase = 0;
	m_dwModuleAddress = 0;
	m_dwModuleSize = 0;
	m_bWriteAccess = bWriteAccess;

	const TCHAR* szProcessName = s_ProcessName;
	DWORD dwProcessId = CProcess::Find(szProcessName);
	if ( dwProcessId == 0 )
	{
		return false;
	}
	
	if ( !CProcess::GetModuleInfo(szProcessName, dwProcessId, &m_dwModuleAddress, &m_dwModuleSize) )
	{
		return false;
	}

	DWORD dwDesiredAccess = PROCESS_DEFAULT_ACCESS;
	if ( m_bWriteAccess )
	{
		dwDesiredAccess |= PROCESS_VM_WRITE | PROCESS_VM_OPERATION;
	}

	if ( Open(dwProcessId, dwDesiredAccess) )
	{
		DWORD dwAddress = FindPattern(m_dwModuleAddress, m_dwModuleSize, (BYTE*)s_BaseSignature, s_BaseMask);
		if ( dwAddress )
		{
			m_dwManagerBase = ReadDWORD(dwAddress - s_BaseOffset);
			return true;
		}
	}

	return false;
}


DWORD CPathOfExile::GetGameBase()
{
	DWORD arOffsets[] = { 0x04, 0x7c };
	return GetMultiLevelPointer32(m_dwManagerBase, arOffsets);
}


DWORD CPathOfExile::GetPlayerBase()
{
	DWORD dwGameBase = GetGameBase();
	if ( dwGameBase )
	{
		DWORD arOffsets[] = { 0x158, 0x5a0 };
		return GetMultiLevelPointer32(dwGameBase + 0x9c, arOffsets);
	}

	return 0;
}


bool CPathOfExile::IsInGame()
{
	DWORD dwGameBase = GetGameBase();
	if ( dwGameBase )
	{
		DWORD arOffsets[] = { 0x158 };
		DWORD dwAddress = GetMultiLevelPointer32(dwGameBase + 0x9c, arOffsets);
		if ( dwAddress != 0 )
		{
			return true;
		}
	}

	return false;
}


bool CPathOfExile::GetPlayerHealth(PLAYERHEALTH* health)
{
	DWORD dwPlayerBase = GetPlayerBase();
	if ( dwPlayerBase )
	{
		DWORD arOffsets[] = {  0xC };
		DWORD dwHealthPtr = GetMultiLevelPointer32(dwPlayerBase + 0x4, arOffsets);
		if ( dwHealthPtr )
		{
			return ReadMemory(dwHealthPtr + 0x50, (LPVOID)health, sizeof(PLAYERHEALTH));
		}
	}

	return false;
}


bool CPathOfExile::GetPlayerExp(PLAYEREXP* exp)
{
	DWORD dwPlayerBase = GetPlayerBase();
	if ( dwPlayerBase )
	{
		DWORD arOffsets[] = { 0x14 };
		DWORD dwStatsPtr = GetMultiLevelPointer32(dwPlayerBase + 0x4, arOffsets);
		if ( dwStatsPtr )
		{
			DWORD dwValue = 0;

			ReadMemory(dwStatsPtr + 0x34, &dwValue, sizeof(dwValue));
			exp->Current = dwValue;

			ReadMemory(dwStatsPtr + 0x44, &dwValue, sizeof(dwValue));
			exp->CurrentLevel = dwValue;

			exp->Minimum = s_ExperienceThresholds[exp->CurrentLevel];
			exp->Maximum = s_ExperienceThresholds[exp->CurrentLevel + 1];

			return true;
		}
	}

	return false;
}


bool CPathOfExile::EnableMapHack(bool bEnable)
{
	DWORD dwAddress = FindPattern(m_dwModuleAddress, m_dwModuleSize, (BYTE*)s_MapSignature, s_MapMask);
	if ( dwAddress )
	{
		if ( ReadByte(dwAddress) == 0x51 )
		{
			while ( ReadByte(dwAddress) != 0xc3 )
			{
				BYTE num1 = ReadByte(dwAddress);
				BYTE num2 = ReadByte(dwAddress + 1);

				if ( bEnable && num1 == 0xd9 && num2 == 0x0 )
				{
					WriteByte(dwAddress + 1, 0xe8);
				}
				else if ( !bEnable && num1 == 0xd9 && num2 == 0xe8 )
				{
					WriteByte(dwAddress + 1, 0x0);
				}

				dwAddress++;
			}

			return true;
		}
	}

	return false;
}