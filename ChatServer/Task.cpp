#include "stdafx.h"
#include "Task.h"
#include "Server.h"
#include "DBModule.h"
#include "Common.h"

int Task::ProcessRecvData(WPARAM wParam, LPARAM lParam)
{
	if (wParam == NULL || lParam == NULL)
		return 0;
	RecvSendData *data = (RecvSendData*)wParam;
	Server *server = (Server *)lParam;
	Document document;
	if (document.ParseInsitu((char*)data->buff).HasParseError())
	{
		delete data;
		return 0;
	}
	if (!(document.IsObject() && document.HasMember("protocol")))
	{
		delete data;
		return 0;
	}

	int protocol = document["protocol"].GetInt();


	switch (protocol)
	{
	case SIGNIN: // 登录
	{
		SignInParam *signInParam = new SignInParam;
		signInParam->connectID = data->connectID;
		signInParam->account = AToW(document["account"].GetString());
		signInParam->password = AToW(document["password"].GetString());
		server->db->SignIn(signInParam);
		delete signInParam;
	}
		break;
	case SIGNUP: // 注册
	{
		User * user = new User;
		user->connectID = data->connectID;
		user->account = AToW(document["account"].GetString());
		user->headerImg = document["headerImg"].GetInt();
		user->area = AToW(document["area"].GetString());
		user->nickName = AToW(document["nickName"].GetString());
		user->password = AToW(document["password"].GetString());
		user->phone = AToW(document["phone"].GetString());
		user->sexulity = document["sexulity"].GetInt();
		user->signature = AToW(document["signature"].GetString());
		server->db->SignUp(user);
		delete user;
	}
		break;
	case GET_FRIENDS: // 获取好友
		
		break;
	case SEARCH_FRIENDS: // 查找好友
	{
		User * user = server->db->GetUser(AToW(document["account"].GetString()));
		if (user == NULL)
		{
			StringBuffer s;
			Writer<StringBuffer> writer(s);
			writer.StartObject();
			writer.Key("protocol");
			writer.Int(SEARCH_FRIENDS);
			writer.Key("result");
			writer.Bool(false);
			writer.EndObject();
			const char *json = s.GetString();
			BYTE *buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
			memcpy_s(buff, strlen(json) + 1, json, strlen(json));
			RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, data->connectID);
			server->network->SendData(sendData);
			break;
		}
		else
		{
			StringBuffer s;
			Writer<StringBuffer> writer(s);
			writer.StartObject();
			writer.Key("protocol");
			writer.Int(SEARCH_FRIENDS);
			writer.Key("result");
			writer.Bool(true);
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
			RecvSendData *sendData = new RecvSendData(buff, strlen(json) + 1, data->connectID);
			server->network->SendData(sendData);
			delete user;
			break;
		}
	}
	case FRIENDS_REQUEST: // 好友请求

		break;
	case 112: // 查找好友

		break;
	case 1111: // 查找好友

		break;
	case 11111: // 查找好友

		break;
	default:
		break;
	}

	delete data;
	return 0;
}

int Task::GetFriends(WPARAM wParam, LPARAM lParam)
{
	if (wParam == NULL || lParam == NULL)
		return 0;
	ULONG userID = ULONG(wParam);
	ULONG connectID = ULONG(lParam);


	return 0;
}
