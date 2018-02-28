#pragma once
#include "stdafx.h"
#include "NetwordModule.h"
#include "DBModule.h"
#include <queue>
#include "ThreadPool.h"

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

	ThreadPool * threadPool;
	DBModule *db;
	NetwordModule *network;
private:
	CRITICAL_SECTION csLock;
	queue<RecvSendData *> queueRecvDatas;

};

