#pragma once

#include "Process.h"


typedef struct _PLAYERSTATS {
	DWORD MaxHP;
	DWORD CurrentHP;
	DWORD ReservedHPFlat;
	DWORD ReservedHPPercent;
	BYTE Unknown1[20];
	DWORD MaxMana;
	DWORD CurrentMana;
	DWORD ReservedManaFlat;
	DWORD ReservedManaPercent;
	BYTE Unknown2[20];
	DWORD MaxShield;
	DWORD CurrentShield;
} PLAYERSTATS;


typedef struct _PLAYEREXPERIENCE {
	DWORD Minimum;
	DWORD Current;
	DWORD Maximum;
	DWORD CurrentLevel;
} PLAYEREXPERIENCE;


class CPathOfExile : public CProcess
{
protected:
	DWORD m_dwModuleSize;
	DWORD m_dwModuleAddress;
	DWORD m_dwBaseManager;
	DWORD m_dwHeroManager;
	bool m_bIsSteamProcess;

public:
	CPathOfExile();
	~CPathOfExile();

	BOOL Attach();

	BOOL IsInGame();
	BOOL GetPlayerStats(PLAYERSTATS* stats);
	BOOL GetPlayerExperience(PLAYEREXPERIENCE* exp);
};