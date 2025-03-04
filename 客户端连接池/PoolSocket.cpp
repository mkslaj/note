#include "PoolSocket.h"
#include <string.h>
#include <pthread.h>
#include <queue>
#include <iostream>
#include <unistd.h>
#include <string.h>
using namespace std;

// Socket连接池结构PoolHandle
struct PoolHandle
{
	queue<TcpSocket*> sockList;			// 存储可以通信的套接字对象
	int				bounds;				// Socket连接池的容量

	string 			serverip;
	unsigned short 	serverport;

	int 			connecttime;
	int				sTimeout; // 没有连接时，等待之间
	pthread_mutex_t foo_mutex;
};

PoolSocket::PoolSocket()
{
}

PoolSocket::~PoolSocket()
{
}

int PoolSocket::poolInit(PoolParam * param)
{
	int ret = 0;
	PoolHandle *hdl = new PoolHandle;
	m_handle = hdl;
	//初始化 句柄
	if (hdl == NULL)
	{
		ret = MallocErr;
		return ret;
	}

	// 数据初始化
	hdl->serverip = param->serverip;
	hdl->serverport = param->serverport;
	hdl->connecttime = param->connecttime;
	//处理连接数
	hdl->bounds = param->bounds;
	hdl->sTimeout = 100;

	pthread_mutex_init(&(hdl->foo_mutex), NULL);

	pthread_mutex_lock(&(hdl->foo_mutex)); //流程加锁
	// 创建用于通信的套接字对象
	connectServer();
	pthread_mutex_unlock(&(hdl->foo_mutex)); //解锁

	return ret;
}

TcpSocket* PoolSocket::getConnect()
{
	PoolHandle *hdl = static_cast<PoolHandle*>(m_handle);
	// 流程加锁 pthread_mutex_unlock(& (hdl->foo_mutex) ); //解锁
	pthread_mutex_lock(&(hdl->foo_mutex)); 

	// 若 有效连数 = 0
	if (hdl->sockList.size() == 0)
	{
		usleep(hdl->sTimeout); //等上几微妙
		// 还是没有可用的连接
		if (hdl->sockList.size() == 0)
		{
			return NULL;
		}
	}
	// 从对头取出一条连接, 并将该节点弹出
	TcpSocket* sock = hdl->sockList.front();
	hdl->sockList.pop();
	cout << "取出一条连接, 剩余连接数: " << curConnSize() << endl;
	
	pthread_mutex_unlock(&(hdl->foo_mutex)); //解锁

	return sock;
}

int PoolSocket::putConnect(TcpSocket* sock, bool isValid)
{
	int	ret = 0;
	PoolHandle *hdl = static_cast<PoolHandle*>(m_handle);
	pthread_mutex_lock(&(hdl->foo_mutex)); //流程加锁 

	// 判断连接是否已经被 放进来 		
	// 判断该连接是否已经被释放
	if (isValid)
	{
		// 连接可用, 放入队列
		hdl->sockList.push(sock);
		cout << "放回一条连接, 剩余连接数: " << curConnSize() << endl;
	}
	else
	{
		// 套接字不可用, 析构对象, 在创建一个新的连接
		sock->disConnect();
		delete sock;
		connectServer(false);
		cout << "修复一条连接, 剩余连接数: " << curConnSize() << endl;
	}
	pthread_mutex_unlock(&(hdl->foo_mutex)); //解锁

	return ret;
}

void PoolSocket::poolDestory()
{
	PoolHandle *hdl = static_cast<PoolHandle*>(m_handle);
	// 遍历队列
	while (hdl->sockList.size() != 0)
	{
		// 取出对头元素
		TcpSocket* sock = hdl->sockList.front();
		// 弹出对头原始
		hdl->sockList.pop();
		// 释放内存
		delete sock;
	}
	delete hdl;
}

int PoolSocket::curConnSize()
{
	PoolHandle *hdl = static_cast<PoolHandle*>(m_handle);
	return hdl->sockList.size();
}

void PoolSocket::connectServer(bool recursion)
{
	PoolHandle *hdl = static_cast<PoolHandle*>(m_handle);
	if ((int)hdl->sockList.size() == hdl->bounds)
	{
		cout << "连接池对象初始化完毕, ^_^ ..." << endl;
		cout << "Poll Size: " << hdl->sockList.size() << endl;
		cout << "Poll bounds: " << hdl->bounds << endl;
		return;
	}
	TcpSocket* socket = new TcpSocket;
	char* ip = const_cast<char*>(hdl->serverip.data());
	int ret = socket->connectToHost(ip, hdl->serverport, hdl->connecttime);
	if (ret == 0)
	{
		// 成功连接服务器
		hdl->sockList.push(socket);
		cout << "Connect count: " << hdl->sockList.size() << endl;
	}
	else
	{
		// 失败
		cout << "连接服务器失败 - index: " << hdl->sockList.size()+1 << endl;
		// 释放对象
		delete socket;
	}
	if (recursion)
	{
		// 递归调用
		connectServer();
	}
}
