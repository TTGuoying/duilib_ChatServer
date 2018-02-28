#include "ServerMainWnd.h"



ServerMainWnd::ServerMainWnd()
{
}


ServerMainWnd::~ServerMainWnd()
{
}

UILIB_RESOURCETYPE ServerMainWnd::GetResourceType() const
{
//#ifdef _DEBUG
//	return UILIB_FILE;
//#else
//	return UILIB_ZIP;
//#endif // _DEBUG
	return UILIB_FILE;
}

CDuiString ServerMainWnd::GetSkinFolder()
{
	return L"Skin\\";
}

CDuiString ServerMainWnd::GetSkinFile()
{
	return L"MainWnd.xml";
}

CDuiString ServerMainWnd::GetZIPFileName() const
{
	return L"Skin.zip";
}

LPCTSTR ServerMainWnd::GetWindowClassName() const
{
	return L"ServerMainWnd";
}

void ServerMainWnd::InitWindow()
{
	lbTips = static_cast<CLabelUI *>(m_PaintManager.FindControl(L"Tips"));

}

void ServerMainWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == L"click")
	{
		if (msg.pSender->GetName() == L"BtnOne")
		{
			if (server.Init())
				ShowTip(L"开启服务器成功");
			return;
		}
		else if (msg.pSender->GetName() == L"BtnTwo")
		{
			return;
		}
		else if (msg.pSender->GetName() == L"BtnThree")
		{
			return;
		}
		else if (msg.pSender->GetName() == L"BtnFour")
		{
			return;
		}
		else if (msg.pSender->GetName() == L"BtnFive")
		{
			return;
		}
		else if (msg.pSender->GetName() == L"BtnSix")
		{
			return;
		}
		else if (msg.pSender->GetName() == L"BtnSeven")
		{
			return;
		}
		else if (msg.pSender->GetName() == L"BtnEight")
		{
			return;
		}
		else if (msg.pSender->GetName() == L"BtnNine")
		{
			return;
		}
	}
	return WindowImplBase::Notify(msg);
}


void ServerMainWnd::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void ServerMainWnd::ShowTip(CDuiString tips, BOOL bBarning)
{
	if (bBarning)
	{
		lbTips->SetText(tips);
		lbTips->SetTextColor(0xFFFF0000);
	}
	else
	{
		lbTips->SetText(tips);
		lbTips->SetTextColor(0xFF5FD557);
	}
}
