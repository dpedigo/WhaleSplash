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
	CButton m_BtnMapHack;
	CComboBox m_ComboMod;
	CEdit m_EditPercent;
	StatusListViewCtrl m_Stats;
	CUpDownCtrl m_SpinPercent;

	CThreadWatch m_WatchThread;

public:
	enum { IDD = IDD_MAIN };

	CMainDlg();

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKeyPress)
		MESSAGE_HANDLER(WM_PLAYER_UPDATE, OnPlayerUpdate)
		COMMAND_HANDLER(IDC_START, BN_CLICKED, OnClickStart)
		COMMAND_HANDLER(IDC_MAPHACK, BN_CLICKED, OnToggleMapHack)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnHotKeyPress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnPlayerUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnClickStart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	LRESULT OnToggleMapHack(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

	TCHAR* StatusCodeToString(DWORD dwStatusCode);
	void ConfigureThread();
};