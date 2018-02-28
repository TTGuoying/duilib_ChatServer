/*
==========================================================================
* 这个类CIOCPModel是本代码的核心类，用于说明WinSock服务器端编程模型中的完成端口(IOCP)的使用方法

* 其中的IOContext类是封装了用于每一个重叠操作的参数

* 具体说明了服务器端建立完成端口、建立工作者线程、投递Recv请求、投递Accept请求的方法，所有的客户端连入的Socket都需要绑定到IOCP上，所有从客户端发来的数据，都会调用回调函数。

*用法：派生一个子类，重载回掉函数

Author: TTGuoying

Date: 2018/02/07 16:22

==========================================================================
*/
#pragma once
#include "Common.h"


// IOCP基类
class IOCPBase
{
public:
	IOCPBase();
	~IOCPBase();

	// 开始服务
	BOOL Start(int port = 10240, int maxConn = 2000, int maxIOContextInPool = 256, 
		int maxSocketContextInPool = 200);
	// 停止服务
	void Stop();
	// 向指定客户端发送数据
	BOOL SendData(RecvSendData *data);

	// 获取本机的IP
	wstring GetLocalIP();

	// 获取当前连接数
	ULONG GetConnectCnt() { return connectCnt;  }

	// 获取当前连接数
	UINT GetPort() { return port; }

	// 事件通知函数(派生类重载此族函数)
	// 新连接
	virtual void OnConnectionEstablished(ULONG connectID) = 0;
	// 连接关闭
	virtual void OnConnectionClosed(ULONG connectID) = 0;
	// 连接上发生错误
	virtual void OnConnectionError(ULONG connectID, int error) = 0;
	// 读操作完成
	virtual void OnRecvCompleted(RecvSendData *data) = 0;
	// 写操作完成
	virtual void OnSendCompleted(ULONG connectID, ULONG dataID) = 0;

private:
	HANDLE					stopEvent;				// 通知线程退出的时间
	HANDLE					completionPort;			// 完成端口
	HANDLE					*workerThreads;			// 工作者线程的句柄指针
	int						workerThreadNum;		// 工作者线程的数量
	wstring					IP;						// 本地IP
	int						port;					// 监听端口
	SocketContext			*listenSockContext;		// 监听socket的Context
	ULONG					connectCnt;				// 当前的连接数量
	ULONG					acceptPostCnt;			// 当前投递的的Accept数量

	vector<SocketContext *> vecSockContexts;			// socketContext列表
	SRWLOCK					srwLockSC;				// socketContext列表读写锁

	LPFN_ACCEPTEX			fnAcceptEx;				//AcceptEx函数指针
	//GetAcceptExSockAddrs;函数指针
	LPFN_GETACCEPTEXSOCKADDRS	fnGetAcceptExSockAddrs;

	static DWORD WINAPI WorkerThreadProc(LPVOID lpParam); // 工作线程函数

	// 初始化IOCP
	BOOL InitializeIOCP();
	// 初始化Socket
	BOOL InitializeListenSocket();
	// 释放资源
	void DeInitialize();
	// socket是否存活
	BOOL IsSocketAlive(SOCKET sock);
	// 获取本机CPU核心数
	int GetNumOfProcessors();
	// 将句柄(Socket)绑定到完成端口中
	BOOL AssociateWithIOCP(SocketContext *sockContext);
	// 投递IO请求
	BOOL PostAccept(SocketContext *sockContext, IOContext *ioContext);
	BOOL PostRecv(SocketContext *sockContext, IOContext *ioContext);
	BOOL PostSend(SocketContext *sockContext, IOContext *ioContext);

	// IO处理函数
	BOOL DoAccpet(SocketContext *sockContext, IOContext *ioContext);
	BOOL DoRecv(SocketContext *sockContext, IOContext *ioContext);
	BOOL DoSend(SocketContext *sockContext, IOContext *ioContext);
	BOOL DoClose(SocketContext *sockContext);

	void AddSocketContext(SocketContext *sockContext);
	void RemoveSocketContext(SocketContext *sockContext);
	SocketContext *GetContextFromID(ULONG id);
};

