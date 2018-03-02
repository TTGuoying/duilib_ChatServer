#pragma once
#include "stdafx.h"
#include "NetwordModule.h"
#include "DBModule.h"
#include "ThreadPool.h"
#include <queue>

using std::queue;

class Server
{
public:
	Server();
	~Server();

	BOOL Start(CDuiString DBpath, UINT port = 10086);
	BOOL Init();
	void DeInit();
	BOOL Stop();

	ThreadPool *threadPool;
	DBModule *db;
	NetwordModule *network;

};

