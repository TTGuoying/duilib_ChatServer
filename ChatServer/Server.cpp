#include "Server.h"



Server::Server()
{
	
}


Server::~Server()
{
	
}

BOOL Server::Start(CDuiString DBpath, UINT port)
{
	return 0;
}

BOOL Server::Init()
{
	network = new NetwordModule(this);
	if (network == NULL || network->Start() == false)
		return false;
	//network->SetSever(this);
	db = new DBModule(this);
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

