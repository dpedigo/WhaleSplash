#pragma once

class StatusListViewCtrl : public CWindowImpl<StatusListViewCtrl, WTL::CListViewCtrl>
{
    TCHAR m_szStatus[500];

public:
	BEGIN_MSG_MAP(StatusListViewCtrl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
    END_MSG_MAP()


    StatusListViewCtrl()
	{
		m_szStatus[0] = NULL;
	}

    
    void Attach(HWND hwnd)
    {
        SubclassWindow(hwnd);
    }


    void SetStatusMessage(TCHAR* szStatus)
    {
		StringCchCopy(m_szStatus, ARRAYSIZE(m_szStatus), szStatus);
		RedrawWindow();
    }


	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		bHandled = FALSE;
        if ( GetItemCount() == 0 )
        {
			int nLength = lstrlen(m_szStatus);
            if ( nLength )
            {
                CRect rectClient;
                GetClientRect(&rectClient);

				CRect rectHeader;
                GetHeader().GetClientRect(&rectHeader);
                
                PAINTSTRUCT ps;
                SIZE size;
                CDCHandle handle = BeginPaint(&ps);
                handle.SelectFont(GetFont());
                handle.GetTextExtent(m_szStatus, nLength, &size);

				rectClient.top += rectHeader.Height() + 20;
                rectClient.bottom = rectClient.top + size.cy;
                handle.DrawText(m_szStatus, -1, &rectClient, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
                
				EndPaint(&ps);
				bHandled = TRUE;
				return 0;
            }
        }

		return 1;
    }
};