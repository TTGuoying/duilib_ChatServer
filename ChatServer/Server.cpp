#include "Server.h"



Server::Server()
{
	InitializeCriticalSection(&csLock);
}


Server::~Server()
{
	DeleteCriticalSection(&csLock);
}

BOOL Server::Start(CDuiString DBpath, UINT port)
{
	return 0;
}

BOOL Server::Init()
{
	network = new NetwordModule;
	if (network == NULL || network->Start() == false)
		return false;
	network->SetSever(this);
	db = new DBModule;
	if (db == NULL || db->Init() == false)
		return false;
	threadPool = new ThreadPool;

	return true;
}

void Server::DeInit()
{

}

BOOL Server::Stop()
{
	return 0;
}

