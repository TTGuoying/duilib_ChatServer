#pragma once
#include "stdafx.h"
#include "SQLite.h"

// 登录信息
struct SignInParam
{
	LPCTSTR		account;
	LPCTSTR		password;
	ULONG		connectID;
};

// 用户
struct User
{
	ULONG		userID;
	int			headerImg;
	LPCTSTR		nickName;
	int			sexulity;
	LPCTSTR		account;
	LPCTSTR		password;
	LPCTSTR		signature;
	LPCTSTR		area;
	LPCTSTR		phone;
	ULONG		connectID;
};

// 登录状态
struct SignInStatus
{
	ULONG		userID;
	BOOL		online;
	ULONG		connectID;
};

// 好友
struct Friend
{
	ULONG		userID;
	ULONG		friendID;
	LPCTSTR		remark;
	LPCTSTR		addTime;
	ULONG		connectID;
};

// 好友聊天记录
struct SingleChatRecord
{
	ULONG		msgID;
	ULONG		userID;
	ULONG		friendID;
	LPCTSTR		msgTime;
	LPCTSTR		message;
	ULONG		connectID;
};

// 好友聊天离线记录
struct SingleUnSendMsg
{
	ULONG		msgID;
	ULONG		connectID;
};

// 群组
struct Group
{
	ULONG		groupID;
	LPCTSTR		groupName;
	ULONG		ownerID;
	int			numOfMember;
	ULONG		connectID;
};

// 群组成员
struct GroupMember
{
	ULONG		groupID;
	ULONG		UserID;
	ULONG		connectID;
};

// 群组聊天记录
struct GroupChatRecord
{
	ULONG		msgID;
	ULONG		groupID;
	ULONG		sendUserID;
	LPCTSTR		msgTime;
	LPCTSTR		message;
	ULONG		connectID;
};

// 群组聊天离线记录
struct GroupUnSendMsg
{
	ULONG		msgID;
	ULONG		UserID;
	ULONG		connectID;
};

class DBModule
{
public:
	DBModule();
	~DBModule();

	BOOL Init();
	BOOL SignIn(SignInParam *param);
	BOOL SignUp(User *user);


	//数据库对象
	SQLite sqliteQuery;
	SQLite sqliteModify;
	CRITICAL_SECTION csLock;
};

