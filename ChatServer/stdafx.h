// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//


#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#define _AFXDLL
//#include "targetver.h"

#include <afxwin.h>
#include <WinSock2.h>
#include <Windows.h>

#include <atlstr.h>
#include <atltime.h>
#include <locale.h>
//使用系统安全字符串函数支持
#include <strsafe.h>
//使用ATL的字符集转换支持
#include <atlconv.h>

#include <MSWSock.h>

#include "DuiLib\UIlib.h"
#ifdef _DEBUG
#       pragma comment(lib, "lib/DuiLib_ud.lib")
#else
#       pragma comment(lib, "lib/DuiLib_u.lib")
#endif
using namespace DuiLib;

#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\reader.h"
using namespace rapidjson;


// TODO: 在此处引用程序需要的其他头文件
#include "Protocol.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
