#pragma once
#include "IOCPBase.h"

class Server;
class NetwordModule : public IOCPBase
{
public:
	NetwordModule(Server *server);
	~NetwordModule();

public:
	// 新连接
	virtual void OnConnectionEstablished(ULONG connectID);
	// 连接关闭
	virtual void OnConnectionClosed(ULONG connectID);
	// 连接上发生错误
	virtual void OnConnectionError(ULONG connectID, int error);
	// 读操作完成
	virtual void OnRecvCompleted(RecvSendData *data);
	// 写操作完成
	virtual void OnSendCompleted(ULONG connectID, ULONG dataID);

	Server *server;
};

