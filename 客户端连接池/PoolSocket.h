#pragma once
#include "TcpSocket.h"
#include <string>
using namespace std;

// 初始化连接池的结构体
struct PoolParam
{
	int 	bounds; //池容量
	int 	connecttime;
	int 	sendtime;
	int 	revtime;
	string 	serverip;
	unsigned short 	serverport;
};

class PoolSocket
{
public:
	enum ErrorType {
		ParamErr = 3000 + 1,
		TimeOut,
		PeerClose,
		MallocErr,
		CreateConnErr,	// 创建连接池 （没有达到最大连接数）
		terminated,		// 已终止
		ValidIsZero,	// 有效连接数是零
		HaveExist,		// 连接已经在池中
		ValidBounds		// 有效连接数目超过了最大连接数
	};
	PoolSocket();
	~PoolSocket();

	int poolInit(PoolParam *param);
	// 从连接池中获取一条连接
	TcpSocket* getConnect();
	// 将连接放回到连接池
	int putConnect(TcpSocket* sock, bool isValid);
	// 释放连接池资源
	void poolDestory();
	int curConnSize();

private:
	void connectServer(bool recursion = true);

private:
	void* m_handle;
};

