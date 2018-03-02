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
	if (sqliteQuery.Open(GetInstancePath() + L"ChatServer.db") == FALSE)
		return FALSE;
	if (sqliteModify.Open(GetInstancePath() + L"ChatServer.db") == FALSE)
	{
		sqliteQuery.Close();
		return FALSE;
	}
	return TRUE;
}

BOOL DBModule::SignIn(SignInParam * param)
{
	User *user = GetUser(param->account);
	if (user == NULL)
	{
		return FALSE;
	}
	if (user->password == param->password)
	{
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

void DBModule::SetSignInStatus(ULONG userID, BOOL online)
{
}

BOOL DBModule::GetSignInStatus(ULONG userID)
{
	return 0;
}

BOOL DBModule::SignUp(User *user)
{
	BOOL bSuccess = FALSE;
	EnterCriticalSection(&csLock);
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select * from User where Account='%s'", user->account);
	SQLiteDataReader reader = sqliteModify.ExcuteQuery(sql);
	if (reader.Read() == NULL)
	{
		StringCbPrintf(sql, 1024, L"insert into User (Account,Password,HeaderImg,NickName,Sexulity,Signature,Area,Phone) VALUES ('%s','%s',%d, '%s',%d,'%s','%s','%s')", user->account, user->password, user->headerImg, user->nickName, user->sexulity, user->signature, user->area, user->phone);
		if (sqliteModify.ExcuteModify(sql))
		{
			bSuccess = TRUE;
		}
	}
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
	reader.Close();
	return bSuccess;
}

BOOL DBModule::GetFriends(ULONG userID, ULONG connectID)
{
	BOOL bSuccess = FALSE;
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select FriendID from Friends where UserID=%d", userID);
	SQLiteDataReader reader = sqliteQuery.ExcuteQuery(sql);
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
	return 0;
}

User *DBModule::GetUser(ULONG userID)
{
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select * from User where UserID=%d", userID);
	SQLiteDataReader reader = sqliteQuery.ExcuteQuery(sql);
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
	StringCbPrintf(sql, 1024, L"select * from User where Account='%s'", account);
	SQLiteDataReader reader = sqliteQuery.ExcuteQuery(sql);
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
