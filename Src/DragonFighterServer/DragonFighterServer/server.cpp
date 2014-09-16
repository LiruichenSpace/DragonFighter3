#include "winsock2.h"
#include "stdlib.h"
#include <string.h>

#include "func.h"

#include <vector>
#include <list>
#include <map>
#include <fstream>
#include<vector>
#include<iostream>
using namespace std;

vector<Player> playerpool;

#define FILENUM 5

char *data[FILENUM];

//目前只支持一组用户攻击
int attack = -1;
int  attackNum = 0;
bool mutex = false;

int SendOnlinePlayerInfo(SOCKET clisock,int currentPlayerIndex);
int SendMessage(SOCKET clisock,int currentPlayerIndex);
int SendPlayer(Player p,SOCKET clisock);
int RecvPlayer(SOCKET clisock, Player &p);

DWORD WINAPI ThreadFunc(LPVOID p);

int AttackPlayer(SOCKET clisock,int index);
int DealPlayerInfo(char *buff,Player &p);
int ActivateOthers(SOCKET clisock, int currentplayerindex);
int UserLogin(SOCKET clisock);
int InitServer()
{
	char name[6] = {'D','A','T','A','1','\0'};
	cout<<"Init Server..."<<endl;
	cout<<"Load Data..."<<endl;
	for (int i =0; i < FILENUM; i++)
	{
		name[4] = itoc(i+1);
		cout<<"Data："<<name<<" Complete !"<<endl;
		data[i] = ReadFileToStr(name,2048);
	}
	cout<<"服务器版本："<<data[0]<<endl;
	cout<<"Init Server Complete!"<<endl;
	Point p1 = {3,5};
	vector<int> g1,m1;

	Player u1 = {"zipxin","123",12,0,0,56,74,p1,"大剑师",132,32,g1,19,43,m1,false};
	Point p2 = {16,9};
	vector<int> g2,m2;
	Player u2 = {"only","456",32,0,0,42,64,p2,"魔导师",132,32,g2,19,43,m2,false};

	playerpool.push_back(u1);
	playerpool.push_back(u2);

	return 1;
}

int UpdateData(SOCKET clisock)
{
	cout<<"The Client Update ..."<<endl;
	char buff[2048];
	char res[16];
	for (int i = 0; i < FILENUM ; i++)
	{
		memset(buff,0,sizeof(buff));
		sprintf_s(buff, "%s",data[i]);
		send(clisock, buff, strlen(buff), 0);
		
		memset(res, 0, sizeof(res));
		if(recv(clisock, res, sizeof(res), 0) == SOCKET_ERROR)
		{
			fprintf(stderr, "update recv error !\n");
			cout<<"The Client has error!!"<<endl;
			return -1;
		}

		if (strcmp(res, "success") == 0)
		{
			cout<<"数据："<<i<<" Update Success！"<<endl;
		}
	}
	return 1;
}

int main(int argc, char* argv[])
{
	SOCKET servsock, clisock;
	struct sockaddr_in sa;
	struct sockaddr_in cliaddr;
	int len;
	int err;
	int servport = 5555;
	WSADATA wsd;

	

	printf("****************************************\n");
	printf("* time server sample                   *\n");
	printf("****************************************\n");	

	if(WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		printf("WSAStartup() failed!\n");
		return -3;
	}

	printf("Creating socket ...\n");
	servsock = socket(AF_INET, SOCK_STREAM, 0);
	if (servsock == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed: %d\n", WSAGetLastError() );
		return -4;
	}
	printf("[0K]\n");

	memset(&sa, 0, sizeof(sa) );
	sa.sin_family = AF_INET;
	sa.sin_port = htons(servport);
	sa.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	
	printf("Bind ...\n");
	err = bind(servsock, (const sockaddr *)&sa, sizeof(sa));
	if(err!=0)
	{
		fprintf(stderr, "bind() failed: %d\n", WSAGetLastError() );
		return -5;
	}
	printf("[OK]\n");

	printf("listen ...\n");
	err = listen(servsock, 5);
	if(err != 0)
	{
		fprintf(stderr, "listen() failed: %d\n", WSAGetLastError() );
		return -6;
	}
	printf("[0k]\n");

	if(!InitServer())
	{
		cout<<"Server has error!!"<<endl;
		getchar();
		closesocket(servsock);
		WSACleanup();
		return 0;
	}

	while(true)
	{
		printf("wait request ...\n");
		len = sizeof(cliaddr);
		clisock = accept(servsock, (struct sockaddr *) &cliaddr, &len);

		if(clisock == INVALID_SOCKET)
		{
			fprintf(stderr, "accept() failed: %d\n", WSAGetLastError());
			closesocket(servsock);
			return -7;
		}
		printf("accept client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port) );
		
		CreateThread(NULL, 0, ThreadFunc, (LPVOID)&clisock, 0, NULL);
	}
	
	closesocket(servsock);
	WSACleanup();
	return 0;
}


DWORD WINAPI ThreadFunc(LPVOID p)
{
	char buff[10240];   //用于 接受 请求
	SOCKET clisock = *(SOCKET *)p;
	//send server's version
	send(clisock, data[0], strlen(data[0]), 0);

	int playindex = -1;
	bool isLogin = false;

	while ( true )  //  与客户机 进行数据交互， break本循环代表 交互结束
	{
		cout<<"Main Operation..."<<endl;

		printf("recv data ..\n");
		memset(buff, 0, sizeof(buff));
		if(recv(clisock, buff, sizeof(buff), 0) == SOCKET_ERROR)
		{
			fprintf(stderr, " recv error !\n");
			break;
		}
		printf("[OK]\n");
		cout<<"接受请求："<<buff<<endl;

		if (strcmp(buff,"update") == 0)
		{
			if(UpdateData(clisock) < 0){ break; }
		} 
		else if(strcmp(buff,"login") == 0)
		{
			if((playindex = UserLogin(clisock))<0) { break; }
			if(playindex != -1) isLogin = true;
		}
		else if(strcmp(buff,"otherinfo") == 0)
		{
			if(ActivateOthers(clisock,playindex)<0) { break; }
		}
		else if(strcmp(buff,"attack") == 0)
		{
			if(AttackPlayer( clisock,playindex) < 0) { break;}
		}
		else
		{
			if(isLogin)
			{
				if(DealPlayerInfo(buff,playerpool[playindex])< 0)
				{
					cout<<"Client has error!"<<endl;
					break;
				}
			}else
			{
				cout<<"Client has error!"<<endl;
				break;
			}

		}
		if(isLogin)
		{
			 SendMessage(clisock,playindex);
		}
	}//while
	
	fprintf(stderr, "The client is disconnected !\n");
	if(isLogin) //处理结束后的代码
	{
		playerpool[playindex].isLogin = false;
	}
	closesocket(clisock);
	return 0;
}

int UserLogin(SOCKET clisock)
{
	char buff[128];
	//获取客户端登录信息
	printf("recv user login info ..\n");
	memset(buff, 0, sizeof(buff));
	if(recv(clisock, buff, sizeof(buff), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, " recv error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}
	printf("[OK]\n");
	cout<<buff<<endl;

	char name[32];
	char psd[32];

	string str(buff);
	int pos = str.find(",");

	//将字符串信息进行拆分 验证
	strcpy_s(name ,sizeof(name),str.substr(0, pos).c_str());
	strcpy_s(psd ,sizeof(psd),str.substr(pos+1, str.length()).c_str());

	printf("recv name: %s\n", &name);
	printf("recv password: %s\n", &psd);

	memset(buff, 0, sizeof(buff));
	for (unsigned int i= 0; i < playerpool.size(); i++)
	{
		cout<<"User Name: "<<playerpool[i].name<<"  User Password: "<<playerpool[i].password<<endl;
		cout<<name<<"  "<<psd<<endl;
		if (strcmp(name, playerpool[i].name) == 0 && strcmp(psd, playerpool[i].password) == 0)
		{
			sprintf_s(buff,"success");
			send(clisock, buff, strlen(buff), 0);
			playerpool[i].isLogin = true;
			SendPlayer(playerpool[i],clisock);
			return i;
		}
	}

	sprintf_s(buff,"failed");
	send(clisock, buff, strlen(buff), 0);

	return 1;
}

int SendPlayer(Player p, SOCKET clisock)
{
	char buff1[10240];
	memset(buff1,0,sizeof(buff1));
	sprintf_s(buff1,"%s;%s;%d;%d;%d;%d;%d;%d;%d;%d",p.name,p.profession,p.AllEx,p.money,p.attack,p.red,p.blue,p.position.x,p.position.y,p.grade);
	send(clisock, buff1, strlen(buff1), 0);
	cout<<buff1<<endl;
	return 1;
}

int RecvPlayer(SOCKET clisock, Player &p)
{
	char buff[10240];
	memset(buff,0,sizeof(buff));
	if(recv(clisock, buff, sizeof(buff), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, " recv error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}
	DealPlayerInfo(buff,p);
	return 1;
}

int DealPlayerInfo(char *buff,Player &p)
{
	string str(buff);

	if(str.find(';',0) == string::npos)
	{
		cout<<"客户端数据异常"<<endl;
		return -1;
	}
	vector<string> info = split(str,';');

	//加上没有玩家数据的判断
	strcpy_s(p.name,sizeof(p.name),info[0].c_str() );
	strcpy_s(p.profession,sizeof(p.profession),info[1].c_str() );
	p.AllEx =  atoi(info[2].c_str());

	p.money = atoi(info[3].c_str());
	p.attack = atoi(info[4].c_str());
	p.red = atoi(info[5].c_str());
	p.blue = atoi(info[6].c_str());
	p.position.x = atoi(info[7].c_str());
	p.position.y = atoi(info[8].c_str());
	p.grade = atoi(info[9].c_str());
	p.isGroup = false;
	p.MaxEx = ExCalcu(p.grade);

	return 1;
}

int SendOnlinePlayerInfo(SOCKET clisock,int currentPlayerIndex)
{
	char buff[10240];
	memset(buff,0,sizeof(buff));
	char buf[256];
	bool hasInfo = false;
	if(!playerpool.empty())
	{
		for(unsigned int i = 0; i < playerpool.size(); i++)
		{
			if(i == currentPlayerIndex || !playerpool[i].isLogin ) continue;
			else
			{
				memset(buf,0,sizeof(buf));
				sprintf_s(buf,"%s,%d,%d,%d;",playerpool[i].name,
										 playerpool[i].position.x,
										 playerpool[i].position.y,
										 playerpool[i].isGroup);
				strcat_s(buff,buf);
				hasInfo = true;
			}
		}
	}
	if (!hasInfo)
	{
		sprintf_s(buff,"noinfo");
	}
	send(clisock,buff, strlen(buff),0);
	cout<<buff<<endl;
	return 1;
}


int ActivateOthers(SOCKET clisock, int currentplayerindex)
{
	cout<<"客户端与其他玩家进行交互"<<endl;
	char name[128];
	//获取客户端发来的交互玩家名称
	printf("recv other user name ..\n");
	memset(name, 0, sizeof(name));
	if(recv(clisock, name, sizeof(name), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, " recv error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}
	printf("[OK]\n");
	cout<<"交互玩家名称："<<name<<endl;

	int otherindex= -1;
	if(!playerpool.empty())
	{
		for (unsigned int i = 0; i<playerpool.size(); i++)
		{
			if (strcmp(name, playerpool[i].name) == 0)
			{
				otherindex = i;
				break;
			}
		}
	}
	char buff[1024];
	memset(buff,0,sizeof(buff));
	if(otherindex == -1||!playerpool[otherindex].isLogin)
	{
		sprintf_s(buff,"noinfo");
		send(clisock,buff, strlen(buff),0);
		return 1;
	}else
	{
		SendPlayer(playerpool[otherindex],clisock);
	}
	return otherindex;
}
int SendMessage(SOCKET clisock,int currentPlayerIndex)
{
	if (attack != -1)
	{
		char buff[128];   //用于 接受 请求
		memset(buff,0,sizeof(buff));
		sprintf_s(buff,"attack");
		send(clisock, buff, sizeof(buff), 0);
		SendPlayer(playerpool[attack],clisock);

		memset(buff,0,sizeof(buff));
		sprintf_s(buff,"%d",attackNum);
		send(clisock, buff, sizeof(buff), 0);
		attack = -1;
		attackNum = 0;
		mutex = false;
		if(SendOnlinePlayerInfo(clisock,currentPlayerIndex)<0 ) return -1;
	}
	else
	{
		if(SendOnlinePlayerInfo(clisock,currentPlayerIndex)<0 ) return -1;
	}
	return 1;
}
int AttackPlayer(SOCKET clisock, int index)
{
	cout<<"客户端攻击其他玩家"<<endl;
	char name[128];
	//获取客户端发来的交互玩家名称
	printf("recv other user name ..\n");
	memset(name, 0, sizeof(name));
	if(recv(clisock, name, sizeof(name), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, " recv error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}
	printf("[OK]\n");
	cout<<"交互玩家名称："<<name<<endl;

	int otherindex= -1;
	if(!playerpool.empty())
	{
		for (unsigned int i = 0; i<playerpool.size(); i++)
		{
			if (strcmp(name, playerpool[i].name) == 0)
			{
				otherindex = i;
				break;
			}
		}
	}
	char buff[1024];
	memset(buff,0,sizeof(buff));
	if(otherindex == -1||!playerpool[otherindex].isLogin)
	{
		sprintf_s(buff,"noinfo");
		send(clisock,buff, strlen(buff),0);
		return 1;
	}else
	{
		sprintf_s(buff,"success");
		send(clisock,buff, strlen(buff),0);
	}

	char att[128];
	memset(att, 0, sizeof(att));
	if(recv(clisock, att, sizeof(att), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, " recv error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}
	printf("[OK]\n");
	attackNum = atoi(att);

	if(attackNum == 0) 
	{
		return 1;
	}else
	{
		mutex = true;
		attack = index;
	}
	while(mutex){};
	return 1;
}