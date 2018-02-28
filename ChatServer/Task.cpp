#include "stdafx.h"
#include "Task.h"
#include "Server.h"
#include "DBModule.h"

int Task::ProcessRecvData(PVOID p)
{
	if (p == NULL)
		return 0;

	ProcessRecvDataParam *param = (ProcessRecvDataParam *)p;
	Document document;
	if (document.ParseInsitu((char *)param->data->buff).HasParseError())
	{
		delete param->data;
		delete param;
		return 0;
	}
	if (!(document.IsObject() && document.HasMember("protocol")))
	{
		delete param->data;
		delete param;
		return 0;
	}

	int protocol = document["protocol"].GetInt();

	User *user = NULL;
	SignInParam *signInParam = NULL;

	switch (protocol)
	{
	case SIGNIN: // µÇÂ¼
		signInParam = new SignInParam;
		signInParam->connectID = param->data->connectID;
		signInParam->account = Utf8ToWchar(document["account"].GetString());
		signInParam->password = Utf8ToWchar(document["password"].GetString());
		if (param->server->db->SignIn(signInParam))
		{
			StringBuffer s;
			Writer<StringBuffer> writer(s);
			writer.StartObject();
			writer.Key("protocol");
			writer.Int(SIGNIN_SECCUSS);
			writer.EndObject();
			const char *json = s.GetString();
			BYTE *data = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
			memcpy_s(data, strlen(json) + 1, json, strlen(json));
			RecvSendData *sendData = new RecvSendData(data, strlen(json) + 1, param->data->connectID);
			param->server->network->SendData(sendData);
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
			BYTE *data = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
			memcpy_s(data, strlen(json) + 1, json, strlen(json));
			RecvSendData *sendData = new RecvSendData(data, strlen(json) + 1, param->data->connectID);
			param->server->network->SendData(sendData);
		}
		delete signInParam;
		break;
	case SIGNUP: // ×¢²á
		user = new User;
		user->connectID = param->data->connectID;
		user->account = Utf8ToWchar(document["account"].GetString());
		user->headerImg = document["headImg"].GetInt();
		user->area = Utf8ToWchar(document["area"].GetString());
		user->nickName = Utf8ToWchar(document["nickName"].GetString());
		user->password = Utf8ToWchar(document["password"].GetString());
		user->phone = Utf8ToWchar(document["phone"].GetString());
		user->sexulity = document["sexulity"].GetInt();
		user->signature = Utf8ToWchar(document["signature"].GetString());
		if (param->server->db->SignUp(user))
		{
			StringBuffer s;
			Writer<StringBuffer> writer(s);
			writer.StartObject();
			writer.Key("protocol");
			writer.Int(SIGNUP_SECCUSS);
			writer.EndObject();
			const char *json = s.GetString();
			BYTE *data = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
			memcpy_s(data, strlen(json) + 1, json, strlen(json));
			RecvSendData *sendData = new RecvSendData(data, strlen(json) + 1, param->data->connectID);
			param->server->network->SendData(sendData);
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
			BYTE *data = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(json) + 1);
			memcpy_s(data, strlen(json) + 1, json, strlen(json));
			RecvSendData *sendData = new RecvSendData(data, strlen(json) + 1, param->data->connectID);
			param->server->network->SendData(sendData);
		}
		delete user;
		break;
	case 100: // µÇÂ¼

		break;
	case 101: // µÇÂ¼

		break;
	default:
		break;
	}

	delete param->data;
	delete param;
	return 0;
}
