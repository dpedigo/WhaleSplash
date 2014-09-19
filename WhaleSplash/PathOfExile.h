#pragma once

#include "Process.h"

typedef struct _PLAYERHEALTH {
	DWORD MaxHP;
	DWORD CurHP;
	DWORD ReservedHPFlat;
	DWORD ReservedHPPercent;
	BYTE Unknown1[20];
	DWORD MaxMana;
	DWORD CurtMana;
	DWORD ReservedManaFlat;
	DWORD ReservedManaPercent;
	BYTE Unknown2[20];
	DWORD MaxShield;
	DWORD CurShield;
} PLAYERHEALTH;

typedef struct _PLAYEREXP {
	DWORD Minimum;
	DWORD Current;
	DWORD Maximum;
	DWORD CurrentLevel;
} PLAYEREXP;


class CPathOfExile : public CProcess
{
private:
	static const TCHAR* s_ProcessName;
	static const DWORD  s_BaseOffset;
	static const char*  s_BaseMask;
	static const BYTE   s_BaseSignature[];
	static const char*  s_MapMask;
	static const BYTE   s_MapSignature[];
	static const DWORD  s_ExperienceThresholds[];

protected:
	DWORD m_dwModuleSize;
	DWORD m_dwModuleAddress;

	DWORD m_dwManagerBase;

	bool m_bWriteAccess;

public:
	CPathOfExile();
	~CPathOfExile();

	bool Attach(bool bWriteAccess = false);

	DWORD GetGameBase();
	DWORD GetPlayerBase();

	bool IsInGame();

	bool GetPlayerHealth(PLAYERHEALTH* health);
	bool GetPlayerExp(PLAYEREXP* exp);

	bool EnableMapHack(bool bEnable);
};