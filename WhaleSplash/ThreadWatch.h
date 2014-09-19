#pragma once

#include "Thread.h"
#include "Process.h"
#include "PathOfExile.h"

class CThreadWatch : public CThreadImpl<CThreadWatch>
{
private:
	HWND m_hWndStatus;
	UINT m_uExitPercent;
	bool m_bLife;
	bool m_bMapHack;

protected:
	void SendPlayerStatus(DWORD dwStatusCode);
	void SendPlayerExp(PLAYEREXP* pExp);

public:
	CThreadWatch();
	DWORD Run();

	void Configure(HWND hWndStatus, UINT uExitPercent, bool bLife, bool bMapHack);
	void EnableMapHack(bool bEnable);
};