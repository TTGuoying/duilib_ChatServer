#pragma once
#include "stdafx.h"
#include "SQLite.h"
#include "Common.h"

// 登录信息
struct SignInParam
{
	CString		account;
	CString		password;
	ULONG		connectID;
};

// 用户
struct User
{
	ULONG		userID;
	int			headerImg;
	CString		nickName;
	int			sexulity;
	CString		account;
	CString		password;
	CString		signature;
	CString		area;
	CString		phone;
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
	CString		remark;
	CString		addTime;
	ULONG		connectID;
};

// 好友聊天记录
struct SingleChatRecord
{
	ULONG		msgID;
	ULONG		userID;
	ULONG		friendID;
	CString		msgTime;
	CString		message;
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
	CString		groupName;
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
	CString		msgTime;
	CString		message;
	ULONG		connectID;
};

// 群组聊天离线记录
struct GroupUnSendMsg
{
	ULONG		msgID;
	ULONG		UserID;
	ULONG		connectID;
};

class Server;
class DBModule
{
public:
	DBModule(Server *server);
	~DBModule();

	BOOL Init();

	// 登录 注册
	BOOL SignIn(SignInParam *param);
	void SetStatusOnline(ULONG userID, ULONG connectID);
	void SetStatusOnline(ULONG connectID);
	BOOL GetSignInStatus(ULONG userID);
	BOOL SignUp(User *user);

	// 获取ConnectID
	ULONG GetConnectID(ULONG userID);

	// 获取好友
	BOOL GetFriends(ULONG userID, ULONG connectID);
	User *GetUser(ULONG userID);
	User *GetUser(CString account);
	int GetFriendNum(ULONG userID);

	// 好友请求
	void InsertFriendRequest(ULONG userID, ULONG friendID, int toFriend, int friendHadle, int toUser);
	void AgreeFriendRequest(ULONG userID, ULONG friendID);
	void RefusetFriendRequest(ULONG userID, ULONG friendID);

	// 聊天信息
	void SingleMsg(SingleChatMsg *msg);

	//数据库对象
	SQLite sqlite;
	CRITICAL_SECTION csLock;
	Server *server;
};

