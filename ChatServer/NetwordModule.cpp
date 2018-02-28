#include "NetwordModule.h"
#include "Server.h"
#include "Task.h"

NetwordModule::NetwordModule()
{
}


NetwordModule::~NetwordModule()
{
}

void NetwordModule::OnConnectionEstablished(ULONG connectID)
{
}

void NetwordModule::OnConnectionClosed(ULONG connectID)
{
}

void NetwordModule::OnConnectionError(ULONG connectID, int error)
{
}

void NetwordModule::OnRecvCompleted(RecvSendData *data)
{
	ProcessRecvDataParam *param = new ProcessRecvDataParam;
	param->data = data;
	param->server = server;
	server->threadPool->QueueTaskItem(Task::ProcessRecvData, param);
}

void NetwordModule::OnSendCompleted(ULONG connectID, ULONG dataID)
{
}

void NetwordModule::SetSever(Server * server)
{
	this->server = server;
}
