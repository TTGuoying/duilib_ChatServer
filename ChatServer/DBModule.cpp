#include "DBModule.h"
#include "Common.h"
#include "Server.h"
#include <vector>

using std::vector;

DBModule::DBModule(Server *server)
{
	InitializeCriticalSection(&csLock);
	this->server = server;
}


DBModule::~DBModule()
{
	DeleteCriticalSection(&csLock);
}

BOOL DBModule::Init()
{
	if (sqlite.Open(GetInstancePath() + L"ChatServer.db") == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL DBModule::SignIn(SignInParam * param)
{
	User *user = GetUser(param->account);
	if (user == NULL)
	{
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("protocol");
		writer.Int(SIGNIN_FAILED);
		writer.EndObject();
		const char *json = s.GetString();
		BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
		memcpy_s(buff, strlen(json) + 1, json, strlen(json));
		RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, param->connectID);
		server->network->SendData(sendData);
		return FALSE;
	}
	if (user->password == param->password)
	{
		ULONG connID = GetConnectID(user->userID);
		if (-1 != connID)
		{
			StringBuffer s;
			Writer<StringBuffer> writer(s);
			writer.StartObject();
			writer.Key("protocol");
			writer.Int(SIGNIN_ALREADY);
			writer.EndObject();
			const char *json = s.GetString();
			BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
			memcpy_s(buff, strlen(json) + 1, json, strlen(json));
			RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, connID);
			server->network->SendData(sendData);
		}
		SetStatusOnline(user->userID, param->connectID);
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("protocol");
		writer.Int(SIGNIN_SECCUSS);
		writer.Key("userID");
		writer.Int(user->userID);
		writer.Key("account");
		writer.String(WToA(user->account));
		writer.Key("headerImg");
		writer.Int(user->headerImg);
		writer.Key("nickName");
		writer.String(WToA(user->nickName));
		writer.Key("sexulity");
		writer.Int(user->sexulity);
		writer.Key("signature");
		writer.String(WToA(user->signature));
		writer.Key("area");
		writer.String(WToA(user->area));
		writer.Key("phone");
		writer.String(WToA(user->phone));
		writer.EndObject();
		const char *json = s.GetString();
		BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
		memcpy_s(buff, strlen(json) + 1, json, strlen(json));
		RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, param->connectID);
		server->network->SendData(sendData);
		GetFriends(user->userID, param->connectID);
		return TRUE;
	}
	else
	{
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("protocol");
		writer.Int(SIGNIN_FAILED);
		writer.EndObject();
		const char *json = s.GetString();
		BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
		memcpy_s(buff, strlen(json) + 1, json, strlen(json));
		RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, param->connectID);
		server->network->SendData(sendData);
		return FALSE;
	}
}

void DBModule::SetStatusOnline(ULONG userID, ULONG connectID)
{
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"UPDATE SignInStatus SET OnLine=1, ConnectID=%d WHERE UserID = %d", connectID, userID);
	EnterCriticalSection(&csLock);
	sqlite.ExcuteModify(sql);
	LeaveCriticalSection(&csLock);
}

void DBModule::SetStatusOnline(ULONG connectID)
{
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"UPDATE SignInStatus SET OnLine=0, ConnectID=-1 WHERE ConnectID = %d", connectID);
	EnterCriticalSection(&csLock);
	sqlite.ExcuteModify(sql);
	LeaveCriticalSection(&csLock);
}

BOOL DBModule::GetSignInStatus(ULONG userID)
{
	BOOL bSuccess = FALSE;
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select OnLine from SignInStatus where UserID = %d", userID);
	SQLiteDataReader reader = sqlite.ExcuteQuery(sql);
	if (reader.Read() != NULL)
	{
		if (reader.GetIntValue(0) != 0)
		{
			bSuccess = TRUE;
		}
	}
	reader.Close();
	return bSuccess;
}

BOOL DBModule::SignUp(User *user)
{
	BOOL bSuccess = FALSE;
	EnterCriticalSection(&csLock);
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select * from User where Account = '%s'", user->account);
	SQLiteDataReader reader = sqlite.ExcuteQuery(sql);
	if (reader.Read() == NULL)
	{
		EnterCriticalSection(&csLock);
		StringCbPrintf(sql, 1024, L"insert into User (Account,Password,HeaderImg,NickName,Sexulity,Signature,Area,Phone) VALUES ('%s','%s',%d, '%s',%d,'%s','%s','%s')", user->account, user->password, user->headerImg, user->nickName, user->sexulity, user->signature, user->area, user->phone);
		if (sqlite.ExcuteModify(sql))
		{
			bSuccess = TRUE;
		}
		User * u = GetUser(user->account);
		StringCbPrintf(sql, 1024, L"insert into SignInStatus VALUES (%d,0,'','',-1)", u->userID);
		delete u;
		if (sqlite.ExcuteModify(sql))
		{
			bSuccess = TRUE;
		}
		LeaveCriticalSection(&csLock);
	}
	reader.Close();
	LeaveCriticalSection(&csLock);
	if (bSuccess)
	{
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("protocol");
		writer.Int(SIGNUP_SECCUSS);
		writer.EndObject();
		const char *json = s.GetString();
		BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
		memcpy_s(buff, strlen(json) + 1, json, strlen(json));
		RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, user->connectID);
		server->network->SendData(sendData);
	}
	else
	{
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("protocol");
		writer.Int(SIGNUP_FAILED);
		writer.EndObject();
		const char *json = s.GetString();
		BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
		memcpy_s(buff, strlen(json) + 1, json, strlen(json));
		RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, user->connectID);
		server->network->SendData(sendData);
	}
	return bSuccess;
}

ULONG DBModule::GetConnectID(ULONG userID)
{
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select OnLine,ConnectID from SignInStatus where UserID = %d", userID);
	SQLiteDataReader reader = sqlite.ExcuteQuery(sql);
	if (reader.Read() != NULL)
	{
		if (reader.GetIntValue(0) != 0)
		{
			ULONG connectID = reader.GetIntValue(1);
			reader.Close();
			return connectID;
		}
	}
	reader.Close();
	return -1;
}

BOOL DBModule::GetFriends(ULONG userID, ULONG connectID)
{
	BOOL bSuccess = FALSE;
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select FriendID from Friends where UserID = %d", userID);
	SQLiteDataReader reader = sqlite.ExcuteQuery(sql);
	vector<ULONG> vecFriendIDs;
	vecFriendIDs.clear();
	while (reader.Read())
	{
		vecFriendIDs.push_back(reader.GetIntValue(0));
	}
	reader.Close();
	vector<User *> vecFriends;
	vecFriends.clear();
	User *user = NULL;
	for (auto it = vecFriendIDs.begin(); it != vecFriendIDs.end(); it++)
	{
		user = GetUser(*it);
		if (user != NULL)
		{
			vecFriends.push_back(user);
		}
	}

	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
	writer.Key("protocol");
	writer.Int(GET_FRIENDS);
	writer.Key("Friends");
	writer.StartArray();
	for (auto it = vecFriends.begin(); it != vecFriends.end(); it++)
	{
		user = *it;
		writer.StartObject();
		writer.Key("userID");
		writer.Int(user->userID);
		writer.Key("account");
		writer.String(WToA(user->account));
		writer.Key("headerImg");
		writer.Int(user->headerImg);
		writer.Key("nickName");
		writer.String(WToA(user->nickName));
		writer.Key("sexulity");
		writer.Int(user->sexulity);
		writer.Key("signature");
		writer.String(WToA(user->signature));
		writer.Key("area");
		writer.String(WToA(user->area));
		writer.Key("phone");
		writer.String(WToA(user->phone));
		writer.EndObject();
	}
	writer.EndArray();
	
	writer.EndObject();
	const char *json = s.GetString();
	BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
	memcpy_s(buff, strlen(json) + 1, json, strlen(json));
	RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, connectID);
	server->network->SendData(sendData);
	vecFriendIDs.clear();
	vecFriends.clear();
	return 0;
}

User *DBModule::GetUser(ULONG userID)
{
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select * from User where UserID = %d", userID);
	SQLiteDataReader reader = sqlite.ExcuteQuery(sql);
	if (reader.Read() == NULL)
	{
		reader.Close();
		return NULL;
	}
	else
	{
		User *user = new User;
		user->userID = userID;
		user->account = reader.GetStringValue(1);
		user->headerImg = reader.GetIntValue(3);
		user->nickName = reader.GetStringValue(4);
		user->sexulity = reader.GetIntValue(5);
		user->signature = reader.GetStringValue(6);
		user->area = reader.GetStringValue(7);
		user->phone = reader.GetStringValue(8);
		reader.Close();
		return user;
	}
}

User * DBModule::GetUser(CString account)
{
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select * from User where Account = '%s'", account);
	SQLiteDataReader reader = sqlite.ExcuteQuery(sql);
	if (reader.Read() == NULL)
	{
		reader.Close();
		return NULL;
	}
	else
	{
		User *user = new User;
		user->userID = reader.GetIntValue(0);
		user->account = reader.GetStringValue(1);
		user->password = reader.GetStringValue(2);
		user->headerImg = reader.GetIntValue(3);
		user->nickName = reader.GetStringValue(4);
		user->sexulity = reader.GetIntValue(5);
		user->signature = reader.GetStringValue(6);
		user->area = reader.GetStringValue(7);
		user->phone = reader.GetStringValue(8);
		reader.Close();
		return user;
	}

}

int DBModule::GetFriendNum(ULONG userID)
{
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select count(*) From Friends where UserID = %d", userID);
	SQLiteDataReader reader = sqlite.ExcuteQuery(sql);
	int cnt = 0;
	if (reader.Read() == NULL)
	{
		reader.Close();
		return 0;
	}
	else
	{
		cnt = reader.GetIntValue(0);
		reader.Close();
		return cnt;
	}
}

void DBModule::InsertFriendRequest(ULONG userID, ULONG friendID, int toFriend, int friendHadle, int toUser)
{
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select * From FriendRequest where UserID = %d and FriendId = %d", userID, friendID);
	SQLiteDataReader reader = sqlite.ExcuteQuery(sql);
	if (reader.Read() == NULL)
	{
		reader.Close();
		StringCbPrintf(sql, 1024, L"insert into FriendRequest VALUES (%d,%d,%d,%d,%d)", userID, friendID, toFriend, friendHadle, toUser);
		EnterCriticalSection(&csLock);
		sqlite.ExcuteModify(sql);
		LeaveCriticalSection(&csLock);
	}
	else
	{
		reader.Close();
		StringCbPrintf(sql, 1024, L"UPDATE FriendRequest SET IsSendToFriend = %d, IsFriendHandle = %d, IsSendToUser = %d where UserID = %d and FriendId = %d", toFriend, friendHadle, toUser, userID, friendID);
		EnterCriticalSection(&csLock);
		sqlite.ExcuteModify(sql);
		LeaveCriticalSection(&csLock);
	}
}

void DBModule::AgreeFriendRequest(ULONG userID, ULONG friendID)
{
	InsertFriendRequest(friendID, userID, 1, 1, 0);	// 更新请求表

	WCHAR sql[1024] = {};
	CDuiString time = GetCurrentTimeString();
	EnterCriticalSection(&csLock);
	StringCbPrintf(sql, 1024, L"insert into Friends VALUES (%d,%d,'','%s')", userID, friendID, time);
	sqlite.ExcuteModify(sql);
	StringCbPrintf(sql, 1024, L"UPDATE User SET Friends = %d where UserID = %d", GetFriendNum(userID), userID);
	sqlite.ExcuteModify(sql);

	StringCbPrintf(sql, 1024, L"insert into Friends VALUES (%d,%d,'','%s')", friendID, userID, time);
	sqlite.ExcuteModify(sql);
	StringCbPrintf(sql, 1024, L"UPDATE User SET Friends = %d where UserID = %d", GetFriendNum(friendID), friendID);
	sqlite.ExcuteModify(sql);
	LeaveCriticalSection(&csLock);	// 插入好友表

	if (GetSignInStatus(userID))
	{
		GetFriends(userID, GetConnectID(userID));
		Sleep(100);
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("protocol");
		writer.Int(AGREE_FRIENDS_REQUEST);
		writer.Key("friendID");
		writer.Int(friendID);
		writer.EndObject();
		const char *json = s.GetString();
		BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
		memcpy_s(buff, strlen(json) + 1, json, strlen(json));
		RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, GetConnectID(userID));
		server->network->SendData(sendData);
	}
	if (GetSignInStatus(friendID))
	{
		GetFriends(friendID, GetConnectID(friendID));
		Sleep(100);
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("protocol");
		writer.Int(AGREE_FRIENDS_REQUEST);
		writer.Key("friendID");
		writer.Int(userID);
		writer.EndObject();
		const char *json = s.GetString();
		BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
		memcpy_s(buff, strlen(json) + 1, json, strlen(json));
		RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, GetConnectID(friendID));
		server->network->SendData(sendData);
		InsertFriendRequest(friendID, userID, 1, 1, 1);	// 更新请求表
	}


}

void DBModule::RefusetFriendRequest(ULONG userID, ULONG friendID)
{
	InsertFriendRequest(friendID, userID, 1, 0, 0);	// 更新请求表
}

void DBModule::SingleMsg(SingleChatMsg * msg)
{
	BOOL bSuccess = FALSE;
	if (GetSignInStatus(msg->to))
	{
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("protocol");
		writer.Int(SINGLE_CHAT_MSG);
		writer.Key("from");
		writer.Int(msg->from);
		writer.Key("msg");
		writer.String(WToA(msg->msg));
		writer.Key("time");
		writer.String(WToA(msg->time));
		writer.EndObject();
		const char *json = s.GetString();
		BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
		memcpy_s(buff, strlen(json) + 1, json, strlen(json));
		RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, GetConnectID(msg->to));
		if (server->network->SendData(sendData))
			bSuccess = TRUE;
	}
	WCHAR sql[1024] = {};
	CDuiString time = GetCurrentTimeString();
	EnterCriticalSection(&csLock);
	StringCbPrintf(sql, 1024, L"insert into SingleChatRecord (FromUserID,ToUserID,MsgTime,Msg,IsSend) VALUES (%d,%d,'%s','%s',%d)", msg->from, msg->to, msg->time, msg->msg, bSuccess?1:0);
	sqlite.ExcuteModify(sql);
	LeaveCriticalSection(&csLock);	// 插入好友表
}
