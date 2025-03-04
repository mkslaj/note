#include <iostream>
#include "PoolSocket.h"
#include <queue>
using namespace std;

int main()
{
	PoolSocket pool;
	PoolParam param;
	param.bounds = 10;
	param.connecttime = 100;
	param.revtime = 100;
	param.sendtime = 100;
	param.serverip = "127.0.0.1";
	param.serverport = 9999;
	pool.poolInit(&param);
	queue<TcpSocket*> list;
	while (pool.curConnSize())
	{
		static int i = 0;
		TcpSocket* sock = pool.getConnect();
		string str = "hello, server ... " + to_string(i++);
		sock->sendMsg((char*)str.c_str(), str.size());
		list.push(sock);
	}
	while (!list.empty())
	{
		TcpSocket* t = list.front();
		pool.putConnect(t, false);
		list.pop();
	}
	cout << "max value: " << pool.curConnSize() << endl;
	return 0;
}