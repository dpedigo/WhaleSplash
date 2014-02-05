#pragma once

#include "CustomListView.h"
#include "ThreadWatch.h"

class CMainDlg : public CDialogImpl<CMainDlg>
{
protected:
	DWORD m_dwStatus;
	DWORD m_dwLevel;
	DWORD m_dwExperience;

	DWORD m_dwStartExperience;
	time_t m_tmStartTime;

	CButton m_BtnStart;
	CComboBox m_ComboMod;
	CEdit m_EditPercent;
	StatusListViewCtrl m_Stats;
	CUpDownCtrl m_SpinPercent;

	CThreadWatch m_WatchThread;

public:
	enum { IDD = IDD_MAIN };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKeyPress)
		MESSAGE_HANDLER(WM_PLAYER_UPDATE, OnPlayerUpdate)
		COMMAND_ID_HANDLER(IDC_START, OnClickStart)
	END_MSG_MAP()


	CMainDlg() : m_dwStatus(0), m_dwLevel(0), m_dwExperience(0), m_dwStartExperience(0), m_tmStartTime(0)
	{

	}


	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CenterWindow();
	
		SetIcon(AtlLoadIcon(IDI_ICON), TRUE);
		SetIcon(AtlLoadIcon(IDI_ICON), FALSE);

		m_BtnStart.Attach(GetDlgItem(IDC_START));

		m_EditPercent.Attach(GetDlgItem(IDC_PERCENT));
		m_EditPercent.SetLimitText(2);

		m_ComboMod.Attach(GetDlgItem(IDC_COMBO));
		m_ComboMod.AddString(_T("Life"));
		m_ComboMod.AddString(_T("Shield"));
		m_ComboMod.SetCurSel(0);

		m_SpinPercent.Attach(GetDlgItem(IDC_SPIN));
		m_SpinPercent.SetPos(40);
		m_SpinPercent.SetRange(1, 99);

		m_Stats.Attach(GetDlgItem(IDC_STATS));
		m_Stats.AddColumn(_T("Stat"), 0);
		m_Stats.AddColumn(_T("Value"), 1);
		m_Stats.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		m_Stats.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
		m_Stats.SetStatusMessage(StatusCodeToString(m_dwStatus));

		RegisterHotKey(m_hWnd, VK_F3, MOD_NOREPEAT, VK_F3);
		RegisterHotKey(m_hWnd, VK_F4, MOD_NOREPEAT, VK_F4);

		return TRUE;
	}


	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if ( m_WatchThread.IsRunning() )
		{
			m_WatchThread.Stop();
		}

		EndDialog(0);
		return TRUE;
	}


	LRESULT OnHotKeyPress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CString szTitle;
		CWindow activeWindow(GetForegroundWindow());
		activeWindow.GetWindowText(szTitle);

		if ( szTitle == _T("Path of Exile") )
		{
			if ( wParam == VK_F4 )
			{
				char input[] = { VK_RETURN, VK_DIVIDE, 'O', 'O', 'S', VK_RETURN };
				SendKeyboardInput(input);
			}
			else if ( wParam == VK_F3 )
			{
				char input[] = { VK_RETURN, VK_DIVIDE, 'I', 'T', 'E', 'M', 'L', 'E', 'V', 'E', 'L', VK_RETURN };
				SendKeyboardInput(input);
			}
		}

		return 0;
	}


	LRESULT OnPlayerUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if ( wParam == PLAYER_UPDATE_EXP )
		{
			m_dwStatus = wParam;
			if ( m_Stats.GetItemCount() == 0 )
			{
				m_Stats.InsertItem(0, _T("Level"));
				m_Stats.InsertItem(1, _T("Experience"));
				m_Stats.InsertItem(2, _T("Time-to-level"));
				m_Stats.SetColumnWidth(0, LVSCW_AUTOSIZE);
			}

			TCHAR szTemp[100];
			PLAYEREXPERIENCE* pExp = (PLAYEREXPERIENCE*)lParam;
			if ( pExp )
			{
				if ( m_dwLevel != pExp->CurrentLevel )
				{
					m_dwLevel = pExp->CurrentLevel;

					StringCchPrintf(szTemp, ARRAYSIZE(szTemp), _T("%u\n"), m_dwLevel);
					m_Stats.SetItemText(0, 1, szTemp);
				}

				if ( m_dwStartExperience == 0 )
				{
					m_dwStartExperience = pExp->Current;
				}

				if ( m_dwExperience != pExp->Current )
				{
					m_dwExperience = pExp->Current;

					StringCchPrintf(szTemp, ARRAYSIZE(szTemp),
						_T("%u/%u (%2.2f%%)"),
						pExp->Current,
						pExp->Maximum,
						(((float)(pExp->Current - pExp->Minimum) / (float)(pExp->Maximum - pExp->Minimum)) * 100)
					);

					m_Stats.SetItemText(1, 1, szTemp);
					m_Stats.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
				}

				// calculate time to level...
				DWORD dwExpRemaining = pExp->Maximum - pExp->Current;
				DWORD dwExpGained = pExp->Current - m_dwStartExperience;

				if ( dwExpGained > 0 )
				{
					double tElapsed = difftime(time(NULL), m_tmStartTime);
					double dwExpPerSecond = dwExpGained / tElapsed;

					double tEstimate = dwExpRemaining / dwExpPerSecond;
					double tHoursRemaining = floor(tEstimate / 3600);
					double tMinutesRemaining = floor(fmod((tEstimate / 60), 60));
					double tSecondsRemaining = fmod(tEstimate, 60);

					StringCchPrintf(szTemp, ARRAYSIZE(szTemp),
						_T("%02.0f:%02.0f:%02.0f"),
						tHoursRemaining,
						tMinutesRemaining,
						tSecondsRemaining
					);

					m_Stats.SetItemText(2, 1, szTemp);
				}
				else
				{
					m_Stats.SetItemText(2, 1, _T("N/A"));
				}

				delete pExp;
			}
		}
		else if ( IS_STATUS_UPDATE(wParam) && m_dwStatus != wParam )
		{
			m_dwLevel = 0;
			m_dwExperience = 0;
			m_dwStartExperience = 0;
			m_dwStatus = wParam;
			m_tmStartTime = time(NULL);

			m_Stats.DeleteAllItems();
			m_Stats.SetStatusMessage(StatusCodeToString(m_dwStatus));
		}

		return 0;
	}


	LRESULT OnClickStart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
	{
		if ( m_WatchThread.IsRunning() )
		{
			m_WatchThread.Stop();

			// reset the UI
			m_EditPercent.EnableWindow(TRUE);
			m_SpinPercent.EnableWindow(TRUE);
			m_ComboMod.EnableWindow(TRUE);

			m_BtnStart.SetWindowText(_T("Start"));

			m_Stats.DeleteAllItems();
			m_Stats.SetStatusMessage(StatusCodeToString(m_dwStatus));
		}
		else
		{
			TCHAR szPercent[5];
			m_EditPercent.GetWindowTextW(szPercent, ARRAYSIZE(szPercent));
			UINT uPercent = (UINT)_ttol(szPercent);

			if ( uPercent <= 0 )
			{
				uPercent = 1;
				m_SpinPercent.SetPos(1);
			}
			else if ( uPercent > 99 )
			{
				uPercent = 99;
				m_SpinPercent.SetPos(99);
			}

			bool bLife = true;
			if ( m_ComboMod.GetCurSel() != 0 )
			{
				bLife = false;
			}

			m_WatchThread.Configure(m_hWnd, uPercent, bLife);
			m_WatchThread.Start();

			// reset the UI
			m_BtnStart.SetWindowText(_T("Stop"));

			m_EditPercent.EnableWindow(FALSE);
			m_SpinPercent.EnableWindow(FALSE);
			m_ComboMod.EnableWindow(FALSE);
		}

		return TRUE;
	}


	void SendKeyboardInput(char* szInput)
	{
		int nCharacters = strlen(szInput);
		if ( nCharacters )
		{
			BlockInput(TRUE);

			INPUT input = { 0 };
			for ( int i = 0; i < nCharacters; i++ )
			{
				input.type = INPUT_KEYBOARD;
				input.ki.wVk = szInput[i];
				input.ki.wScan = 0;
				input.ki.dwFlags = 0;
				input.ki.time = 0;
				input.ki.dwExtraInfo = GetMessageExtraInfo();

				SendInput(1, &input, sizeof(INPUT));
				input.ki.dwFlags |= KEYEVENTF_KEYUP;
				SendInput(1, &input, sizeof(INPUT));
			}

			BlockInput(FALSE);
		}
	}


	TCHAR* StatusCodeToString(DWORD dwStatusCode)
	{
		switch ( dwStatusCode )
		{
		case STATUS_STOPPED:
			return _T("Set parameters and press 'Start'");
		case STATUS_WAITING_POE:
			return _T("Waiting for Path of Exile process...");
		case STATUS_NOT_IN_LEVEL:
			return _T("Waiting for player to enter game...");
		case STATUS_NO_STATS:
			return _T("Cannot locate player stats");
		default:
			return _T("Unknown game status");
		}
	}
};