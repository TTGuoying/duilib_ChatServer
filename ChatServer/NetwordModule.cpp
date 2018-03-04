#include "NetwordModule.h"
#include "Server.h"
#include "Task.h"

NetwordModule::NetwordModule(Server *server)
{
	this->server = server;
}


NetwordModule::~NetwordModule()
{
}

void NetwordModule::OnConnectionEstablished(ULONG connectID)
{
}

void NetwordModule::OnConnectionClosed(ULONG connectID)
{
	server->threadPool->QueueTaskItem(Task::SetOffLineStatus, (WPARAM)connectID, (LPARAM)server);
}

void NetwordModule::OnConnectionError(ULONG connectID, int error)
{
	server->threadPool->QueueTaskItem(Task::SetOffLineStatus, (WPARAM)connectID, (LPARAM)server);
}

void NetwordModule::OnRecvCompleted(RecvSendData *data)
{
	server->threadPool->QueueTaskItem(Task::ProcessRecvData, (WPARAM)data, (LPARAM)server);
}

void NetwordModule::OnSendCompleted(ULONG connectID, ULONG dataID)
{
}

