#include "stdafx.h"
#include <tchar.h>
#include "ServerMainWnd.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	HRESULT hr = ::CoInitialize(NULL);
	if (FAILED(hr))
		return 0;

	ServerMainWnd *mainWnd = new ServerMainWnd;
	mainWnd->Create(NULL, _T("ÁÄÌì·þÎñÆ÷"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	mainWnd->CenterWindow();
	mainWnd->ShowModal();

	::CoUninitialize();
	return 0;
}