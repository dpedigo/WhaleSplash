#pragma once

#include "Thread.h"
#include "Process.h"
#include "PathOfExile.h"

class CThreadWatch : public CThreadImpl<CThreadWatch>
{
private:
	HWND m_hWndStatus;
	UINT m_uLifePercent;
	bool m_bLife;

protected:
	void SendPlayerStatus(DWORD dwStatusCode);
	void SendPlayerExperience(PLAYEREXPERIENCE* pExperience);

public:
	CThreadWatch();
	DWORD Run();

	void Configure(HWND hWndStatus, UINT uLifePercent, bool bLife);
};