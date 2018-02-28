#pragma once
#include "stdafx.h"
#include <vector>
#include <list>
#include <string>

using std::vector;
using std::list;
using std::wstring;

#define INIT_IOCONTEXT_NUM (100)				// IOContextPool中的初始数量
#define BUFF_SIZE (1024*4)						// I/O 请求的缓冲区大小
#define WORKER_THREADS_PER_PROCESSOR (2)		// 每个处理器上的线程数
#define MAX_POST_ACCEPT (10)					// 同时投递的Accept数量
#define EXIT_CODE	(-1)						// 传递给Worker线程的退出信号
#define DEFAULT_IP	(L"127.0.0.1")				// 默认IP地址
#define DEFAULT_PORT	(10240)					// 默认端口

// 释放指针的宏
#define RELEASE(x)			{if(x != NULL) {delete x; x = NULL;}}
// 释放句柄的宏
#define RELEASE_HANDLE(x)	{if(x != NULL && x != INVALID_HANDLE_VALUE) { CloseHandle(x); x = INVALID_HANDLE_VALUE; }}
// 释放Socket的宏
#define RELEASE_SOCKET(x)	{if(x != INVALID_SOCKET) { closesocket(x); x = INVALID_SOCKET; }}

#ifndef TRACE
#include <atltrace.h>

#define TRACE							AtlTrace
#define TRACE0(f)						TRACE(f)
#define TRACE1(f, p1)					TRACE(f, p1)
#define TRACE2(f, p1, p2)				TRACE(f, p1, p2)
#define TRACE3(f, p1, p2, p3)			TRACE(f, p1, p2, p3)
#define TRACE4(f, p1, p2, p3, p4)		TRACE(f, p1, p2, p3, p4)
#define TRACE5(f, p1, p2, p3, p4, p5)	TRACE(f, p1, p2, p3, p4, p5)
#endif


const char* WcharToUtf8(const wchar_t *pwStr);
const wchar_t* Utf8ToWchar(const char *pStr);
CDuiString GetInstancePath();
CDuiString GetCurrentTimeString();

enum IO_OPERATION_TYPE
{
	NULL_POSTED,		// 用于初始化，无意义
	ACCEPT_POSTED,		// 投递Accept操作
	SEND_POSTED,		// 投递Send操作
	RECV_POSTED,		// 投递Recv操作
};

class IOContext
{
public:
	WSAOVERLAPPED		overLapped;		// 每个socket的每一个IO操作都需要一个重叠结构
	SOCKET				ioSocket;		// 此IO操作对应的socket
	WSABUF				wsaBuf;			// 数据缓冲
	IO_OPERATION_TYPE	ioType;			// IO操作类型
	DWORD				numOfBytes;		// 收发的字节数
	ULONG				sendDataID;		// 发送数据包的ID

	IOContext()
	{
		ZeroMemory(&overLapped, sizeof(overLapped));
		ioSocket = INVALID_SOCKET;
		wsaBuf.buf = (char *)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, BUFF_SIZE);
		wsaBuf.len = BUFF_SIZE;
		ioType = NULL_POSTED;
		numOfBytes = 0;
		sendDataID = 0;
	}

	~IOContext()
	{
		RELEASE_SOCKET(ioSocket);

		if (wsaBuf.buf != NULL)
			::HeapFree(::GetProcessHeap(), 0, wsaBuf.buf);
	}

	void Reset()
	{
		if (wsaBuf.buf != NULL)
			ZeroMemory(wsaBuf.buf, BUFF_SIZE);
		else
			wsaBuf.buf = (char *)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, BUFF_SIZE);
		ZeroMemory(&overLapped, sizeof(overLapped));
		ioType = NULL_POSTED;
		numOfBytes = 0;
		sendDataID = 0;
	}
};

// 空闲的IOContext管理类(IOContext池)
class IOContextPool
{
private:
	list<IOContext *> contextList;
	CRITICAL_SECTION csLock;

public:
	IOContextPool()
	{
		InitializeCriticalSection(&csLock);
		contextList.clear();

		EnterCriticalSection(&csLock);
		for (size_t i = 0; i < INIT_IOCONTEXT_NUM; i++)
		{
			IOContext *context = new IOContext;
			contextList.push_back(context);
		}
		LeaveCriticalSection(&csLock);

	}

	~IOContextPool()
	{
		EnterCriticalSection(&csLock);
		for (list<IOContext *>::iterator it = contextList.begin(); it != contextList.end(); it++)
		{
			delete (*it);
		}
		contextList.clear();
		LeaveCriticalSection(&csLock);

		DeleteCriticalSection(&csLock);
	}

	// 分配一个IOContxt
	IOContext *AllocateIoContext()
	{
		IOContext *context = NULL;

		EnterCriticalSection(&csLock);
		if (contextList.size() > 0) //list不为空，从list中取一个
		{
			context = contextList.back();
			contextList.pop_back();
		}
		else	//list为空，新建一个
		{
			context = new IOContext;
		}
		LeaveCriticalSection(&csLock);

		return context;
	}

	// 回收一个IOContxt
	void RecycleIOContext(IOContext *pContext)
	{
		pContext->Reset();
		EnterCriticalSection(&csLock);
		contextList.push_front(pContext);
		LeaveCriticalSection(&csLock);
	}
};

// 空闲的connectIDPool管理类(connectID池)
class ULongIDPool
{
private:
	list<ULONG> IDList;
	CRITICAL_SECTION csLock;
	ULONG idCnt;

public:
	ULongIDPool()
	{
		idCnt = 0;
		InitializeCriticalSection(&csLock);
		IDList.clear();

		EnterCriticalSection(&csLock);
		for (size_t i = 0; i < INIT_IOCONTEXT_NUM; i++)
		{
			IDList.push_back(i+1);
			idCnt++;
		}
		LeaveCriticalSection(&csLock);

	}

	~ULongIDPool()
	{
		EnterCriticalSection(&csLock);
		IDList.clear();
		LeaveCriticalSection(&csLock);

		DeleteCriticalSection(&csLock);
	}

	// 分配一个ID
	ULONG AllocateID()
	{
		ULONG id = 0;

		EnterCriticalSection(&csLock);
		if (IDList.size() > 0) //list不为空，从list中取一个
		{
			id = IDList.front();
			IDList.pop_front();
		}
		else	//list为空，新建一个
		{
			idCnt++;
			id = idCnt;
		}
		LeaveCriticalSection(&csLock);

		return id;
	}

	// 回收一个ID
	void RecycleID(ULONG id)
	{
		EnterCriticalSection(&csLock);
		IDList.push_back(id);
		LeaveCriticalSection(&csLock);
	}
};


class SocketContext
{
public:
	SOCKET connSocket;						// 连接的socket
	SOCKADDR_IN clientAddr;					// 连接的远程地址
	ULONG		connectID;					// 连接的ID

private:
	//vector<IOContext*> arrIoContext;		// 同一个socket上的多个IO请求
	static IOContextPool ioContextPool;		// 空闲的IOContext池
	static ULongIDPool IDPool;				// 空闲的IOContext池
	//CRITICAL_SECTION csLock;

public:
	SocketContext()
	{
		//InitializeCriticalSection(&csLock);
		//arrIoContext.clear();
		connSocket = INVALID_SOCKET;
		ZeroMemory(&clientAddr, sizeof(clientAddr));
		connectID = IDPool.AllocateID();
	}

	~SocketContext()
	{
		IDPool.RecycleID(connectID);
		RELEASE_SOCKET(connSocket);

		// 回收所有的IOContext
		/*for (vector<IOContext*>::iterator it = arrIoContext.begin(); it != arrIoContext.end(); it++)
		{
			ioContextPool.RecycleIOContext(*it);
		}

		EnterCriticalSection(&csLock);
		arrIoContext.clear();
		LeaveCriticalSection(&csLock);

		DeleteCriticalSection(&csLock);*/
	}

	// 获取一个新的IoContext
	IOContext *AllocateIoContext()
	{
		IOContext *context = ioContextPool.AllocateIoContext();
		/*if (context != NULL)
		{
			EnterCriticalSection(&csLock);
			arrIoContext.push_back(context);
			LeaveCriticalSection(&csLock);
		}*/
		context->ioSocket = this->connSocket;
		return context;
	}

	// 从数组中移除一个指定的IoContext
	void RecycleIOContext(IOContext* pContext)
	{
		ioContextPool.RecycleIOContext(pContext);
		/*for (vector<IOContext*>::iterator it = arrIoContext.begin(); it != arrIoContext.end(); it++)
		{
			if (pContext == *it)
			{

				EnterCriticalSection(&csLock);
				arrIoContext.erase(it);
				LeaveCriticalSection(&csLock);

				break;
			}
		}*/
	} 
};

// 网络收发的数据
class RecvSendData
{
public:
	RecvSendData(BYTE *data, size_t bytes, ULONG connectID, ULONG sendDataID = 0)
	{
		buff = data;
		this->bytes = bytes;
		this->sendDataID = sendDataID;
		this->connectID = connectID;
	}

	RecvSendData(IOContext *ioContext, ULONG connectID, ULONG sendDataID = 0)
	{
		buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ioContext->numOfBytes);
		bytes = ioContext->numOfBytes;
		memcpy_s(buff, bytes, ioContext->wsaBuf.buf, bytes);
		this->sendDataID = sendDataID;
		this->connectID = connectID;
	}

	~RecvSendData()
	{
		if (buff)
			::HeapFree(::GetProcessHeap(), 0, buff);
		bytes = 0;
	}

	BYTE		*buff;
	size_t		bytes;
	ULONG		sendDataID;
	ULONG		connectID;
};

class Server;
struct ProcessRecvDataParam
{
	Server *server;
	RecvSendData *data;
};