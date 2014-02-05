#include "stdafx.h"
#include "ThreadWatch.h"


CThreadWatch::CThreadWatch()
{

}


DWORD CThreadWatch::Run()
{
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
					PLAYERSTATS stats = { 0 };
					if ( poe.GetPlayerStats(&stats) )
					{
						DWORD dwCurrentValue = 0;
						DWORD dwMaximumValue = 0;

						if ( m_bLife )
						{
							dwCurrentValue = stats.CurrentHP;
							dwMaximumValue = stats.MaxHP;
						}
						else
						{
							dwCurrentValue = stats.CurrentShield;
							dwMaximumValue = stats.MaxShield;
						}

						DWORD dwCurrentPercent = (DWORD)(((float)dwCurrentValue / (float)dwMaximumValue) * 100);
						if ( stats.CurrentHP > 0 && m_uLifePercent > dwCurrentPercent && dwCurrentPercent > 0 )
						{
							poe.Terminate();
							break;
						}

						// only send status once a second (give or take)
						if ( GetTickCount() > (dwTicks + 500) )
						{
							dwTicks = GetTickCount();

							PLAYEREXPERIENCE exp = { 0 };
							if ( poe.GetPlayerExperience(&exp) )
							{
								SendPlayerExperience(&exp);
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

					if ( IsAborted() )
					{
						goto Exit;
					}

					Sleep(200);
				}

				if ( IsAborted() )
				{
					goto Exit;
				}

				Sleep(200);
			}
		}

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


void CThreadWatch::SendPlayerExperience(PLAYEREXPERIENCE* pExperience)
{
	PLAYEREXPERIENCE* pSendExp = NULL;
	if ( pExperience )
	{
		pSendExp = new PLAYEREXPERIENCE;
		memcpy(pSendExp, pExperience, sizeof(PLAYEREXPERIENCE));
	}

	PostMessage(m_hWndStatus, WM_PLAYER_UPDATE, (WPARAM)PLAYER_UPDATE_EXP, (LPARAM)pSendExp);
}


void CThreadWatch::Configure(HWND hWndStatus, UINT uLifePercent, bool bLife)
{
	m_hWndStatus = hWndStatus;
	m_uLifePercent = uLifePercent;
	m_bLife = bLife;
}