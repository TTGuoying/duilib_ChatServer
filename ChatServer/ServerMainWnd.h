#pragma once
#include "stdafx.h"
#include "Server.h"

class ServerMainWnd : public WindowImplBase
{
public:
	ServerMainWnd();
	~ServerMainWnd();

	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual LPCTSTR GetWindowClassName() const;

	virtual void InitWindow();
	virtual void Notify(TNotifyUI &msg);
	virtual void OnFinalMessage(HWND hWnd);

	void ShowTip(CDuiString tips = L"", BOOL bBarning = FALSE);
	Server server;


	CLabelUI * lbTips;
};

