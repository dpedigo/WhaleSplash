#include "stdafx.h"
#include "DlgMain.h"

CAppModule _Module;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	AtlInitCommonControls(ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES);

	HRESULT hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	CMainDlg dlgMain;
	int nRetVal = dlgMain.DoModal();

	_Module.Term();

	return nRetVal;
}