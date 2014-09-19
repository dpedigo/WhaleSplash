#include "stdafx.h"
#include "ThreadWatch.h"

CThreadWatch::CThreadWatch() : m_bMapHack(false), m_hWndStatus(0)
{

}


DWORD CThreadWatch::Run()
{
Start:
	bool bMapHackEnabled = false;
	DWORD dwTicks = GetTickCount();

	while ( !IsAborted() )
	{
		SendPlayerStatus(STATUS_WAITING_POE);

		CPathOfExile poe;
		if ( poe.Attach() )
		{
			while ( poe.IsRunning() )
			{
				SendPlayerStatus(STATUS_NOT_IN_LEVEL);

				while ( poe.IsInGame() )
				{
					PLAYERHEALTH health = { 0 };
					if ( poe.GetPlayerHealth(&health) )
					{
						DWORD dwCurrentValue = 0;
						DWORD dwMaximumValue = 0;

						if ( m_bLife )
						{
							dwCurrentValue = health.CurHP;
							dwMaximumValue = health.MaxHP;
						}
						else
						{
							dwCurrentValue = health.CurShield;
							dwMaximumValue = health.MaxShield;
						}

						DWORD dwCurrentPercent = (DWORD)(((double)dwCurrentValue / (double)dwMaximumValue) * 100);
						if ( health.CurHP > 0 && m_uExitPercent > dwCurrentPercent && dwCurrentPercent > 0 )
						{
							poe.Terminate();
							goto Start;
						}

						// only send status once a second (give or take)
						if ( GetTickCount() > (dwTicks + 500) )
						{
							dwTicks = GetTickCount();

							PLAYEREXP stats = { 0 };
							if ( poe.GetPlayerExp(&stats) )
							{
								SendPlayerExp(&stats);
							}
							else
							{
								SendPlayerStatus(STATUS_NO_STATS);
							}
						}
					}
					else
					{
						SendPlayerStatus(STATUS_NO_STATS);
					}

					
					if ( bMapHackEnabled != m_bMapHack )
					{
						EnableMapHack(m_bMapHack);
						bMapHackEnabled = m_bMapHack;
					}

					if ( IsAborted() )
						goto Exit;

					Sleep(200);
				}

				if ( IsAborted() )
					goto Exit;

				Sleep(200);
			}
		}

		bMapHackEnabled = false;

		Sleep(1000);
	}

Exit:
	SendPlayerStatus(STATUS_STOPPED);
	return 0;
}


void CThreadWatch::SendPlayerStatus(DWORD dwStatusCode)
{
	static DWORD dwPrevStatus = 0;
	if ( dwStatusCode != dwPrevStatus )
	{
		dwPrevStatus = dwStatusCode;
		PostMessage(m_hWndStatus, WM_PLAYER_UPDATE, (WPARAM)dwStatusCode, NULL);
	}
}


void CThreadWatch::SendPlayerExp(PLAYEREXP* pExp)
{
	PLAYEREXP* pSendExp = NULL;
	if ( pExp )
	{
		pSendExp = new PLAYEREXP;
		memcpy(pSendExp, pExp, sizeof(PLAYEREXP));
	}

	PostMessage(m_hWndStatus, WM_PLAYER_UPDATE, (WPARAM)PLAYER_UPDATE_EXP, (LPARAM)pSendExp);
}


void CThreadWatch::Configure(HWND hWndStatus, UINT uExitPercent, bool bLife, bool bMapHack)
{
	m_hWndStatus = hWndStatus;
	m_uExitPercent = uExitPercent;
	m_bLife = bLife;
	m_bMapHack = bMapHack;
}


void CThreadWatch::EnableMapHack(bool bEnable)
{
	CPathOfExile poe;
	if ( poe.Attach(true) )
	{
		poe.EnableMapHack(bEnable);
	}
}