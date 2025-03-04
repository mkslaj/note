#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "SecKeyShm.h"
using namespace std;

SecKeyShm::SecKeyShm(int key):ShareMemory(key)
{
	
}

SecKeyShm::SecKeyShm(int key, int maxNode):
	ShareMemory(key, sizeof(int) + maxNode*sizeof(NodeSHMInfo)), m_maxNode(maxNode)
{
	//连接共享内存
	void *p = mapShm();
	
	//初始化共享内存
	memset(p, 0x00, sizeof(int) + maxNode*sizeof(NodeSHMInfo));
	
	//将maxNode写入共享内存的头4个字节
	memcpy(p, &m_maxNode, sizeof(int));
	
	//断开与共享内存的连接
	unmapShm();
}

SecKeyShm::SecKeyShm(const char* pathName): ShareMemory(pathName)
{
	
}

SecKeyShm::SecKeyShm(const char* pathName, int maxNode):
	ShareMemory(pathName, sizeof(int) + maxNode*sizeof(NodeSHMInfo)), m_maxNode(maxNode)
{
	//连接共享内存
	void *p = mapShm();
	
	//初始化共享内存
	memset(p, 0x00, sizeof(int) + maxNode*sizeof(NodeSHMInfo));
	
	//将maxNode写入共享内存的头4个字节
	memcpy(p, &m_maxNode, sizeof(int));
	
	//断开与共享内存的连接
	unmapShm();
}

SecKeyShm::~SecKeyShm()
{
	
}

int SecKeyShm::shmWrite(NodeSHMInfo* pNodeInfo)
{
	//连接共享内存
	void *p = mapShm();
	
	//从头4个字节获取maxNode
	memcpy(&m_maxNode, p, sizeof(int));
	cout << "maxNode:" << m_maxNode << endl;
	
	NodeSHMInfo *pNode = (NodeSHMInfo *)((char *)p+sizeof(int));
	
	//先查找原有的
	int i = 0;
	for(i=0; i<m_maxNode; i++)
	{
		if(strcmp(pNodeInfo->clientID, pNode[i].clientID)==0 &&
		   strcmp(pNodeInfo->serverID, pNode[i].serverID)==0)
		{
			memcpy(&pNode[i], pNodeInfo, sizeof(NodeSHMInfo));
			break;
		}
	}
	
	//没找到原有的, 找一个空闲可用的
	NodeSHMInfo tmp;
	memset(&tmp, 0x00, sizeof(NodeSHMInfo));
	if(i==m_maxNode)
	{
		for(i=0; i<m_maxNode; i++)
		if(memcmp(&pNode[i], &tmp, sizeof(NodeSHMInfo))==0) 
		{
			memcpy(&pNode[i], pNodeInfo, sizeof(NodeSHMInfo));
			break;
		}
	}
	
	//断开与共享内存的关联
	unmapShm();
	
	//没有空闲位置可用
	if(i==m_maxNode)
	{
		cout << "no space to use" << endl;
		return -1;
	}
	
	return 0;
}

int SecKeyShm::shmRead(const char* clientID, const char* serverID, NodeSHMInfo* pNodeInfo)
{
	//连接共享内存
	void *p = mapShm();
	
	//从头4个字节获取maxNode
	memcpy(&m_maxNode, p, sizeof(int));
	cout << "maxNode:" << m_maxNode << endl;
	
	NodeSHMInfo *pNode = (NodeSHMInfo *)((char *)p+sizeof(int));
	
	int i = 0;
	for(i=0; i<m_maxNode; i++)
	{
		if(strcmp(clientID, pNode[i].clientID)==0 &&
		   strcmp(serverID, pNode[i].serverID)==0)
		{
			memcpy(pNodeInfo, &pNode[i], sizeof(NodeSHMInfo));
			break;
		}
	}
	
	//断开与共享内存的关联
	unmapShm();
	
	//没有找到节点
	if(i==m_maxNode)
	{
		cout << "not found" << endl;
		return -1;
	}
	
	return 0;
}

void SecKeyShm::printShm()
{
	//连接共享内存
	void *p = mapShm();
	
	//从头4个字节获取maxNode
	memcpy(&m_maxNode, p, sizeof(int));
	cout << "maxNode:" << m_maxNode << endl;
	
	NodeSHMInfo *pNode = (NodeSHMInfo *)((char *)p+sizeof(int));
	
	int i = 0;
	for(i=0; i<m_maxNode; i++)
	{
		cout << "-------" << i << "-------" << endl;
		cout << "status:" << pNode[i].status << endl;
		cout << "seckeyID:" << pNode[i].seckeyID << endl;
		cout << "clientID:" << pNode[i].clientID << endl;
		cout << "serverID:" << pNode[i].serverID << endl;
		cout << "seckey:" << pNode[i].seckey << endl;
	}
	
	return;
}
