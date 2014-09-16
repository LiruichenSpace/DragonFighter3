#include "winsock2.h"
#include "stdlib.h"
#include <string.h>
#include "func.h"
#include <fstream>
#include <vector>
#include <iostream>
using namespace std;

#define FILENUM 5

SOCKET sock;
bool isLogin = false;

Point p1 = {3,5};
vector<int> g,m;
Player CurrentPlayer = {"zipxin",12,false,false,100,74,p1,"大剑师",32,32,g,19,43,m};

vector<Magic> magics;
vector<NPC>  npcs;
vector<Place> places;
vector<Goods> goods;
vector<string> tile;
vector<string> show;
vector<MapBuf> mapbuf;
vector<OnlinePlayer> others;

Map *map;

char* DATA[FILENUM];
int DiscardGoods();
int SendPlayer(Player p);
int RecvPlayer(Player &p);
int Login();

void ToInfo(char *str);
int LoadData();
int DealData(char* buff);
int UpdateData();
int DealPlace();
int CheckUpdate();
void Release();
int BeforeGameOperation();
void BeforeLoginUI();

void MainUI();
int GameOperationLoop();
int UseGoods();
int GoodsLoop();
int BuyGoods();
void GoodsUI();
void PersionalInfoUI();
int UpdateMap();
int DealPlayerInfo(char *buff,Player &p);

int GoToCity(Place p);
void CityUI(Place p);
int RecvMessage();

/*id: 1上 2下 3左 4右*/
int MoveCheck(Point p, int id);
int CheckNear(Point p);
int CheckPosition(Point p);

void AttackUI(Player p1, Player p2);

char info[1024];

int Defense();

int OtherInfoUI(Player p, int funcid);
int DeleteGoods(char *name);
int ActivateOther(char* name, int size);
int Attack(Player p1, Player p2);
int main(int argc, char* argv[])
{
	//定义并且初始化必要的数据
	struct sockaddr_in sa;
	int err;
	int servport = 5555;

	WSADATA wsd;
	unsigned long uladdr;


	//地址缓冲区，暂存输入的地址
	char addr_buf[20];
	cout<<"输入连接地址：";
	cin>>addr_buf;
	//进行地址转化，估计是二进制码！
	uladdr = inet_addr(addr_buf);

	//windows 下启动网络 ,完成对Winsock服务初始化
	if(WSAStartup(MAKEWORD(1,1), &wsd) != 0)
	{
		printf("WSAStartup() failed !\n");
		return -3;
	}

	//创建网络接口
	printf("create Socket ...\n");
	sock = socket(AF_INET, SOCK_STREAM, 0);
	//连接Internet【唯一】 ，指定socket 类型， 指定协议 
	if(sock == INVALID_SOCKET) //如果创建失败
	{
		fprintf(stderr, "Socket() failed: %d\n", WSAGetLastError() );
		return -4;
	}
	printf("[OK]\n");

	//初始化sockaddr_in结构
	memset(&sa, 0, sizeof(sa) );
	sa.sin_family = AF_INET;         //代表TCP/IP协议下的 地址组
	sa.sin_port = htons(servport);   //指定端口号
	sa.sin_addr.S_un.S_addr = uladdr;//知道服务端地址

	//连接操作
	printf("connecting ...\n");
	//将服务器地址信息与socket信息赋给connect函数进行连接
	err = connect(sock, (const sockaddr *) &sa, sizeof(sa));
	if (err !=0)//如果连接出错，返回值不为0
	{
		fprintf(stderr, "connect() failed: %d\n", WSAGetLastError());
		pause();
		return -5;
	}
	printf("[OK]\n");

	if( CheckUpdate()<0 )
	{
		Release();
		return 0;
	}
	pause();
	system("cls");
	LoadData();
	getchar();
	system("cls");
	
	BeforeGameOperation();

	if(isLogin)
		GameOperationLoop();
	
	Release();
	return 0;
}


inline void CutLine(const char *title)
{
	if(title)
		cout<<">>"<<title<<show[3].c_str()<<endl;
	else
		cout<<show[4].c_str()<<endl;
}



int GameOperationLoop()
{
	char input[64];
	ToInfo(" .沙漠 *草地 #城镇 +墓地 ^山峰 =河流 \1自己 @玩家");
	while(true)
	{
		if( RecvMessage() < 0 ) return -1;
		system("cls");
		UpdateMap();
		MainUI();
		memset(input,0,sizeof(input));
		cout<<"请输入指令：";
		gets_s(input,sizeof(input));
		ToInfo("  .沙漠 *草地 #城镇 +墓地 ^山峰 =河流 \1自己 @玩家");
		if(strcmp(input,"w") == 0)
		{
			if(MoveCheck(CurrentPlayer.position,1) > 0)
				CurrentPlayer.position.y --;
			CheckNear(CurrentPlayer.position);
			CheckPosition(CurrentPlayer.position);
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"s") == 0)
		{
			if(MoveCheck(CurrentPlayer.position,2) > 0)
				CurrentPlayer.position.y ++ ;
			CheckNear(CurrentPlayer.position);
			CheckPosition(CurrentPlayer.position);
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"a") == 0)
		{
			if(MoveCheck(CurrentPlayer.position,3) > 0)
				CurrentPlayer.position.x --;
			CheckNear(CurrentPlayer.position);
			CheckPosition(CurrentPlayer.position);
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"d") == 0)
		{
			if(MoveCheck(CurrentPlayer.position,4) > 0)
				CurrentPlayer.position.x ++;
			CheckNear(CurrentPlayer.position);
			CheckPosition(CurrentPlayer.position);
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"info") == 0)
		{
			PersionalInfoUI();
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"prop") == 0)
		{
			GoodsLoop();
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"exit") == 0)
		{
			cout<<"正在退出..."<<endl;
			return 1;
		}
		else 
		{
			ToInfo("  非法输入！");
			SendPlayer(CurrentPlayer);
		}
	}

	return 1;
}
int CheckPosition(Point p)
{
	int index = map->map[p.y][p.x];
	unsigned int i = 0;
	if(index == 6)
	{
		for (i = 0; i< places.size(); i++)
		{
			if (places[i].pos == p)
			{
				GoToCity(places[i]);
				break;
			}
		}
	}
	if (index == 9)
	{
		if(!others.empty())
			for (i = 0; i < others.size(); i++)
			{
				if(others[i].pos == p)
				{
					ActivateOther(others[i].name,sizeof(others[i].name));
					break;
				}
			}
	}
	
	return 1;
}
int GoToCity(Place p)
{
	char input[64];
	ToInfo(" 进入城镇,请选择操作，如果要和NPC对话，直接输入NPC姓名。");
	while(true)
	{
		system("cls");
		CityUI(p);
		memset(input,0,sizeof(input));
		cout<<"请输入指令：";
		gets_s(input,sizeof(input));
		ToInfo(" 进入城镇,请选择操作，如果要和NPC对话，直接输入NPC姓名。");
		if(strcmp(input,"use") == 0)
		{
			 if(GoodsLoop()<0) return -1;
		}
		else if(strcmp(input,"info") == 0)
		{
			 PersionalInfoUI();
		}
		else if(strcmp(input,"buy") == 0)
		{
			 BuyGoods();
		}
		else if(strcmp(input,"hotel") == 0)
		{
			if(CurrentPlayer.money < 20)
			{
				ToInfo(" 对不起，你的钱不够支付你一晚的旅店费用！");
			}
			else
			{
				CurrentPlayer.money -= 20;
				CurrentPlayer.red = 100;
				CurrentPlayer.blue = 100;
				ToInfo(" 进入旅店消费20元，休息了一夜，回复了血量和魔力！");
			}
		}
		else if(strcmp(input,"exit") == 0)
		{
			cout<<"正在退出..."<<endl;
			break;
		}
		else 
		{
			ToInfo("  非法输入！");
			if (!p.NPC.empty())
			{
				for (unsigned int i = 0; i <p.NPC.size(); i++ )
				{
					if(strcmp(input,p.NPC[i].name) == 0)
					{
						char* buf = append(p.NPC[i].name," : ");
						char* buf2 =append(p.NPC[i].name, p.NPC[i].speak);
						ToInfo(buf2);
						break;
					}
				}//npcs
			}// npc not is emply
		}//maybe legel input

	}
	return 1;
}
int CheckNear(Point p)
{
	unsigned int i= 0;
	int up = 0, down = 0, left = 0, right = 0;
	if (p.y - 1 >= 0)           up   = map->map[p.y - 1][p.x];
	if (p.y + 1 < map->height)  down = map->map[p.y + 1][p.x];
	if (p.x - 1 >= 0)           left = map->map[p.y][p.x - 1];
	if (p.x + 1 < map->width)   right= map->map[p.y][p.x + 1];
	
	Point u = {p.x, p.y - 1};
	Point d = {p.x,p.y + 1};
	Point l = {p.x - 1 ,p.y};
	Point r = {p.x + 1 ,p.y};

	if (up == 9 || down == 9 || left == 9 || right == 9)
	{
		for (i = 0; i< others.size(); i++)
		{
			if (others[i].pos == u || 
				others[i].pos == d ||
				others[i].pos == l ||
				others[i].pos == r )
			{
				ToInfo(others[i].name);
				return 1;
			}
		}
	}
	if (up == 6 || down == 6 || left == 6 || right == 6)
	{
		for (i = 0; i< places.size(); i++)
		{
			if (places[i].pos == u || 
				places[i].pos == d ||
				places[i].pos == l ||
				places[i].pos == r )
			{
				ToInfo(places[i].name);
				return 1;
			}
		}
	}
	return 1;
}
int MoveCheck(Point p, int id)
{
	int index = 0;
	if (id == 1)
	{
		if(p.y -1 < 0) 
		{
			ToInfo("错误：超出地图边界！");
			return -1;
		}
		index = map->map[p.y - 1][p.x];
	}
	else if (id == 2)
	{
		if(p.y + 2 >= map->height) 
		{
			ToInfo("错误：超出地图边界！");
			return -1;
		}
		index = map->map[p.y + 1][p.x];
	}
	else if (id == 3)
	{
		if(p.x - 1 < 0) 
		{
			ToInfo("错误：超出地图边界！");
			return -1;
		}
		index = map->map[p.y][p.x - 1];
	}
	else if (id == 4)
	{
		if(p.x + 2 >= map->width) 
		{
			ToInfo("错误：超出地图边界！");
			return -1;
		}
		index = map->map[p.y][p.x + 1];
	}
	else 
	{
		ToInfo("异常：错误的方向id！");
		return -1;
	}

	if (index > 0 && index <=6)
	{
		return 1;
	}
	if(index == 9) return 1;

	if (index == 7)
	{
		ToInfo("不能通过山峰！");
		return -1;
	}
	else if (index == 8)
	{
		ToInfo("不能通过河流！");
		return -1;
	}
	else if (index == 9)
	{
		return 1;
	}else 
	{
		ToInfo("异常：未知地图！！！");
		return -1;
	}
}


int DealData(char *buff)
{
	string data(buff);
	vector<string> modules = split(data,'$');
	unsigned int i = 0, j = 0, k = 0;
	for (i = 0; i<modules.size(); i++)
	{
		vector<string> items = split(modules[i],'%');
		//strcpy_s(name,items[0].size(),items[0].c_str());
		//name = (char*)items[0].c_str();
		if(strcmp(items[0].c_str(),"npc") == 0)
		{
			for (j = 1; j<items.size(); j++)
			{
				NPC newnpc ;
				memset(&newnpc, 0, sizeof(NPC));
				 split(items[j],',');
				vector<string> npcinfo = split(items[j],',');
				newnpc.id = atoi(npcinfo[0].c_str());
				strcpy_s(newnpc.name,sizeof(newnpc.name),npcinfo[1].c_str() );
				strcpy_s(newnpc.speak,sizeof(newnpc.speak),npcinfo[2].c_str() );
				npcs.push_back(newnpc);
			}
		}
		else if (strcmp(items[0].c_str(),"place") == 0)
		{
			for (j = 1; j<items.size(); j++)
			{
				Place newplace ;
				memset(&newplace, 0, sizeof(Place));
				vector<string> info = split(items[j],',');
				newplace.id = atoi(info[0].c_str());
				strcpy_s(newplace.name,sizeof(newplace.name),info[1].c_str() );
				strcpy_s(newplace.construct,sizeof(newplace.construct),info[2].c_str() );
				places.push_back(newplace);
			}
		}
		else if (strcmp(items[0].c_str(),"tile") == 0)
		{
			tile = items;
		}
		else if (strcmp(items[0].c_str(),"goods") == 0)
		{
			for (j = 1; j<items.size(); j++)
			{
				Goods g;
				memset(&g, 0, sizeof(Goods));
				vector<string> info = split(items[j],',');
				g.id = atoi(info[0].c_str());
				strcpy_s(g.name,sizeof(g.name),info[1].c_str() );
				strcpy_s(g.construct,sizeof(g.name),info[2].c_str() );
				g.functionId =  atoi(info[3].c_str());
				g.effect	 =  atoi(info[4].c_str());
				g.price		 =  atoi(info[5].c_str());
				goods.push_back(g);
			}
		}//goods
		else if (strcmp(items[0].c_str(),"magic") == 0)
		{
			for (j = 1; j<items.size(); j++)
			{
				Magic m;
				memset(&m, 0, sizeof(Magic));
				vector<string> info = split(items[j],',');
				m.id = atoi(info[0].c_str());
				strcpy_s(m.name,sizeof(m.name),info[1].c_str() );
				strcpy_s(m.construct,sizeof(m.construct),info[2].c_str() );
				m.effect = atoi(info[3].c_str());
				m.grade = atoi(info[4].c_str());
				magics.push_back(m);
			}
		}//magic
		else if (strcmp(items[0].c_str(),"show") == 0)
		{
			show = items;
		}
		else if (strcmp(items[0].c_str(),"pn") == 0)
		{
			for (j = 1; j<items.size(); j++)
			{
				vector<string> info = split(items[j],',');
				int id = atoi(info[0].c_str());
				for (unsigned int p = 1; p<info.size(); p++)
				{
					int index = atoi(info[p].c_str());
					places[id].NPC.push_back(npcs[index]);
				}
			}
		}//pn place - npc 对应关系
		else if (strcmp(items[0].c_str(),"pp") == 0)
		{
			for (j = 1; j<items.size(); j++)
			{
				vector<string> info = split(items[j],',');
				int id = atoi(info[0].c_str());
				int x = atoi(info[1].c_str());
				int y = atoi(info[2].c_str());
				places[id].pos.x = x;
				places[id].pos.y = y;
			}
		}//pp place's pos
		else if (strcmp(items[0].c_str(),"pg") == 0)
		{
			for (j = 1; j<items.size(); j++)
			{
				vector<string> info = split(items[j],',');
				int id = atoi(info[0].c_str());
				for (unsigned int p = 1; p<info.size(); p++)
				{
					int index = atoi(info[p].c_str());
					places[id].goods.push_back(goods[index]);
				}
			}
		}//pg place - goods 对应关系
		else
		{
			cout<<"数据处理异常!"<<endl;
			cout<<"异常数据："<<items[0].c_str()<<" 无法识别!"<<endl;
			return -1;
		}
		
	}//modules
	//delete buff;
	return 1;
}

int UpdateMap()
{
	unsigned int i = 0;
	if (!mapbuf.empty())
	{
		for (i = 0; i < mapbuf.size(); i++)
		{
			if(mapbuf[i].index != 10)
				map->map[mapbuf[i].y][mapbuf[i].x] = mapbuf[i].index;
		}
	}
	int x = 0, y = 0;
	x = CurrentPlayer.position.x;
	y = CurrentPlayer.position.y;

	//cout<<"Current Player Position x:"<<x<<" y:"<<y<<endl;

	MapBuf mybuf;
	mybuf.x = x; mybuf.y = y;
	mybuf.index = map->map[y][x];
	mapbuf.push_back(mybuf);
	map->map[y][x] = 10;

	//cout<<mybuf.x<<mybuf.y<<(char)mybuf.index<<endl;

	if(!others.empty())
	{
		for (i = 0; i < others.size(); i++)
		{
			MapBuf buf;
			buf.x = others[i].pos.x;
			buf.y = others[i].pos.y;
			buf.index = map->map[others[i].pos.y][others[i].pos.x];
			mapbuf.push_back(buf);
			map->map[others[i].pos.y][others[i].pos.x] = 9;
		}
	}

	return 1;
}

int UseGoods()
{
	char input[64];
	cout<<"请输入使用道具的编号：";
	gets_s(input,sizeof(input));
	int i = atoi(input) - 1;
	if(i == -1 || i<0 || i >(int) CurrentPlayer.goods.size())
	{
		ToInfo("  错误的道具编号！");
	}else
	{
		ToInfo(goods[CurrentPlayer.goods[i]].construct);
		int funcid = goods[CurrentPlayer.goods[i]].functionId;
		if(funcid == 1)
		{
			CurrentPlayer.red += goods[CurrentPlayer.goods[i]].effect;
			if(CurrentPlayer.red > 100)CurrentPlayer.red = 100;
		}else if(funcid == 2)
		{
			CurrentPlayer.blue += goods[CurrentPlayer.goods[i]].effect;
			if(CurrentPlayer.blue > 100)CurrentPlayer.blue = 100;
		}
		else if(funcid == 3)
		{
			CurrentPlayer.attack += goods[CurrentPlayer.goods[i]].effect;
		}
		else 
		{
			ToInfo("  异常错误：道具功能出现异常，使该道具失去效果！");
		}
		DeleteGoods(goods[CurrentPlayer.goods[i]].name);
	}
	return 1;
}

int DiscardGoods()
{
	char input[64];
	cout<<"请输入使用道具的编号：";
	gets_s(input,sizeof(input));
	int i = atoi(input) - 1;
	if(i == -1 || i<0 || i >(int) CurrentPlayer.goods.size())
	{
		ToInfo("  错误的道具编号！");
	}else
	{	
		DeleteGoods(goods[CurrentPlayer.goods[i]].name);
		ToInfo("  删除成功!");
	}
	return 1;
}
void GoodsUI()
{
	CutLine("玩家信息");
	PersionInfo(CurrentPlayer);
	CutLine("物品信息");
	DisplayGoods(goods,CurrentPlayer.goods);
	CutLine("游戏提示");
	HintGoods();
	CutLine("游戏信息");
	if(info) cout<<info<<endl;
	else cout<<endl;
	CutLine(0);
}

int GoodsLoop()
{
	char input[64];
	ToInfo(" 道具处理界面！");
	while(true)
	{
		system("cls");
		GoodsUI();
		memset(input,0,sizeof(input));
		cout<<"请输入指令：";
		gets_s(input,sizeof(input));
		ToInfo(" 道具处理界面！");
		if(strcmp(input,"use") == 0)
		{
			if(CurrentPlayer.goods.empty()) continue;
			else UseGoods();
		}
		else if(strcmp(input,"discard") == 0)
		{
			if(CurrentPlayer.goods.empty()) continue;
			else DiscardGoods();
		}
		else if(strcmp(input,"exit") == 0)
		{
			cout<<"正在退出..."<<endl;
			break;
		}
		else 
		{
			ToInfo("  非法输入！");
		}
			 
	}
	return 1;
}
int UpdateData()
{
	char buff[16];
	memset(buff,0,sizeof(buff));
	sprintf_s(buff, "update");
	send(sock, buff, strlen(buff), 0);

	char name[6] = {'D','A','T','A','1','\0'};
	char data[2048];
	cout<<"Update Data...."<<endl;
	for (int i = 1; i<= FILENUM; i++)
	{
		memset(data, 0, sizeof(data));
		if(recv(sock, data, sizeof(data), 0) == SOCKET_ERROR)
		{
			fprintf(stderr, "update recv error !\n");
			cout<<"The Server has error!!"<<endl;
			return -1;
		}
		name[4] = itoc(i);

		//cout<<data<<endl;

		WriteStrToFile(name, (const char*)data, 2048);

		cout<<"Update data: "<<name<<" complete!"<<endl;
		memset(buff,0,sizeof(buff));
		sprintf_s(buff, "success");
		send(sock, buff, strlen(buff), 0);
	}
	cout<<"Data Update Complete!"<<endl;
	return 1;
}
int BuyGoods()
{	
	char* blank="                           \0";
	unsigned int i = 0;
	system("cls");
	CutLine("道具商城");
	cout<<blank<<"@";
	for(i = 0;i < 20; i++)
		cout<<"-";
	cout<<"@"<<endl;

	if(goods.empty()) cout<<blank<<"      没有物品"<<endl;
	else
	{
		for(i = 0; i < goods.size(); i++)
		{
			cout<<blank<<i+1<<"."<<goods[i].name<<endl;
		}
	}
	cout<<blank<<"@";
	for(i = 0;i < 20; i++)
		cout<<"-";
	cout<<"@"<<endl;
	char input[64];
	cout<<"请输入购买道具的编号：";
	gets_s(input,sizeof(input));
	int k = atoi(input) - 1;
	if(k == -1 || k<0 || k >(int) goods.size())
	{
		ToInfo("  错误的道具编号！");
	}else
	{
		if(CurrentPlayer.money < 20) ToInfo("  你没有足够的现金！");
		else
		{
			CurrentPlayer.goods.push_back(k);
			CurrentPlayer.money -= 20;
			ToInfo("  购买成功！");
		}
	}
	return 1;
}

int LoadData()
{
	char name[6] = {'D','A','T','A','1','\0'};
	cout<<"加载资源..."<<endl;
	for (int i =0; i < FILENUM; i++)
	{
		name[4] = itoc(i+1);
		cout<<"Data："<<name<<" Complete !"<<endl;
		DATA[i] = ReadFileToStr(name,2048);
	}
	map = ReadMapFromFile("DATA3");
	cout<<"正在处理资源..."<<endl;
	if(DealData(DATA[1]) < 0) return -1;
	if(DealData(DATA[3]) < 0) return -1;
	if(DealData(DATA[4]) < 0) return -1;
	DealPlace();
	cout<<"资源处理完成"<<endl;
	cout<<"当前客户端版本："<<DATA[0]<<endl;
	cout<<"资源加载完成！"<<endl;
	return 1;
}

inline void HintMain()
{
	cout<<show[2].c_str()<<endl;
}

int CheckUpdate()
{
	cout<<"Check Update..."<<endl;
	char *version = ReadFileToStr("DATA1", 32);
	char buff[16];
	memset(buff, 0, sizeof(buff));
	if(recv(sock, buff, sizeof(buff), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, "recv error !\n");
		cout<<"The Server has error!!"<<endl;
		return -1;
	}
	cout<<"Client Version:"<<version<<endl;
	cout<<"Server Version:"<<buff<<endl;

	if(strcmp(buff,version) != 0)
	{
		if(UpdateData()<0)  return -1;
	}else
	{
		cout<<"The current version is the latest！"<<endl;
	}
	return 1;
}

int BeforeGameOperation()
{
	char input[64];
	ToInfo(" 游戏登录界面！");
	while(true)
	{
		system("cls");
		BeforeLoginUI();
		memset(input,0,sizeof(input));
		cout<<"请选择:";
		gets_s(input,sizeof(input));
		ToInfo(" 请选择操作！");
		if(strcmp(input,"login") == 0)
		{
			if(Login() < 0) break;
		}
		else if(strcmp(input,"create") == 0)
		{
			ToInfo(" 暂时未开发！");
		}
		else if(strcmp(input,"exit") == 0)
		{
			cout<<"正在退出..."<<endl;
			break;
		}else
		{
			ToInfo(" 非法操作！");
		}
		getchar();
	}//while date interactive
	getchar();
	return 1;
}
void Release()
{
	cout<<"-----------------Disconnect!-----------------"<<endl;
	//断开连接
	closesocket(sock);
	WSACleanup();
	pause();
}

void ToInfo(char *str)
{
	memset(info, 0, sizeof(info));
	strcpy_s(info,sizeof(info),str);
}



void MainUI()
{
	CutLine("玩家信息");
	PersionInfo(CurrentPlayer);
	CutLine("世界地图");
	DisplayMap(map, tile, CurrentPlayer.position);
	CutLine("游戏提示");
	HintMain();
	CutLine("游戏信息");
	if(info) cout<<info<<endl;
	else cout<<endl;
	CutLine(0);
}

void PersionalInfoUI()
{
	system("cls");
	CutLine("玩家信息");
	DisplayPlayerInfo(CurrentPlayer);
	CutLine("游戏提示");
	HintPersionalInfo();
	CutLine(0);
	getchar();
}

int DealPlace()
{
	if(!places.empty())
	{
		for (unsigned int i = 0; i < places.size(); i++)
		{
			map->map[places[i].pos.y][places[i].pos.x] = 6;
		}
	}
	return 1;
}
void CityUI(Place p)
{
	CutLine("城镇名称");
	cout<<"                                     "<<p.name<<endl;
	CutLine("城镇简介");
	cout<<" "<<p.construct<<endl;
	CutLine("操作选项");
	CityMenu();
	CutLine("城镇居民");
	char* blank="                           \0";
	if(!p.NPC.empty())
	{
		unsigned int i = 0;
		cout<<blank<<"@";
		for(i = 0;i < 20; i++)
			cout<<"-";
		cout<<"@"<<endl;
		for (i = 0; i <p.NPC.size(); i++ )
		{
			cout<<blank<<" $姓名："<<p.NPC[i].name<<endl;
		}
		cout<<blank<<"@";
		for(i = 0;i < 20; i++)
			cout<<"-";
		cout<<"@"<<endl;
	}else
	{
		cout<<blank<<" 没有居民"<<endl;
	}
	CutLine("游戏信息");
	if(info) cout<<info<<endl;
	else cout<<endl;
	CutLine(0);
}
int SendPlayer(Player p)
{
	char buff1[10240];
	sprintf_s(buff1,"%s;%s;%d;%d;%d;%d;%d;%d;%d;%d",p.name,p.profession,p.AllEx,p.money,p.attack,p.red,p.blue,p.position.x,p.position.y,p.grade);
	send(sock, buff1, strlen(buff1), 0);
	return 1;
}

int RecvPlayer(Player &p)
{
	char buff[10240];
	memset(buff,0,sizeof(buff));
	if(recv(sock, buff, sizeof(buff), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, " recv error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}
	DealPlayerInfo(buff,p);
	return 1;
}


int Login()
{
	char buff[128];
	memset(buff, 0, sizeof(buff));
	printf("login...\n");//客户端进行登录
	sprintf_s(buff, "%s", "login");
	send(sock, buff, strlen(buff), 0);

	char name[20];
	char password[20];

	memset(name, 0, sizeof(name));
	memset(password, 0, sizeof(password)); //对变量的初始化操作

	cout<<"please input your name:";
	cin>>name;

	cout<<"Please input your password:";
	cin>>password;

	memset(buff, 0, sizeof(buff));
	sprintf_s(buff, "%s,%s", name, password);//输入密码和用户名，并将其组合成字符串

	send(sock, buff, strlen(buff), 0);  //将组合成的字符串传递给服务器
	printf("[send info OK]\n");

	printf("recv data ..\n");           //准备获取服务器的验证信息
	memset(buff, 0, sizeof(buff));
	if(recv(sock, buff, sizeof(buff), 0) == SOCKET_ERROR)//如果获取异常 则退出
	{
		fprintf(stderr, " recv error !\n");
		getchar();
		getchar();
		Release();
		exit(0);
	}
	printf("[OK]\n");

	char login_flag[10];            
	memset(login_flag, 0, sizeof(login_flag));
	sscanf_s(buff, "%s", &login_flag, _countof(login_flag));
	//获取服务器登录状态信息，并赋予login_flag数组

	//判断登录是否成功
	if( strcmp(login_flag, "success") == 0) //如果成功
	{
		printf("Login Success!\n");
		RecvPlayer(CurrentPlayer);
		isLogin = true;
		return -1;
	}else
	{
		cout<<"Login Failed!"<<endl;
		return 1;
	}
}
void BeforeLoginUI()
{
	cout<<show[1].c_str()<<endl;
	CutLine("游戏信息");
	if(info) cout<<info<<endl;
	else cout<<endl;
	CutLine(0);
}
int RecvMessage()
{
	char buff[10240];
	memset(buff, 0, sizeof(buff));
	others.clear();
	if(recv(sock, buff, sizeof(buff), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, "recv server info error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}
	if(strcmp(buff,"noinfo") == 0)
	{
		return 1;
	}

	if(strcmp(buff,"attack") == 0)
	{
		if(Defense() <0) return -1;
		else return 1;
	}

	string s(buff);
	if(s.find(';',0) != string::npos)
	{
		vector<string> info = split(s,';');
		if(!info.empty())
		for(unsigned int i = 0; i<info.size(); i++)
		{
			vector<string> item = split(info[i],',');
			OnlinePlayer p;
			strcpy_s(p.name,sizeof(p.name),item[0].c_str() );
			p.pos.x = atoi(item[1].c_str());
			p.pos.y = atoi(item[2].c_str());
			int group = atoi(item[3].c_str());
			if(group == 0) p.isGroup = false;
			else p.isGroup = true;
			others.push_back(p);
		}
	}else
	{
		ToInfo("服务器数据异常！");
	}

	return 1;
}
int ActivateOther(char* name,int size)
{
	//===================让服务器调用其他玩家交互信息功能模块
	char buff[128];
	memset(buff, 0, sizeof(buff));
	sprintf_s(buff, "%s", "otherinfo");
	send(sock, buff, strlen(buff), 0);
	//===================发送其他玩家信息
	send(sock, name, size, 0);

	char res[1024];
	memset(res,0,sizeof(res));
	if(recv(sock, res, sizeof(res), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, "the server has error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}
	printf("[OK]\n");
	//cout<<res<<endl;
	if(strcmp("noinfo",res) == 0)
	{
		OtherInfoUI(CurrentPlayer,0);
		return 1;
	}
	Player op;
	
	if(DealPlayerInfo(res,op)<0)
	{
		return -1;
	}
	ToInfo(" 玩家交互页面！");
	char input[64];
	while(true)
	{
		if( RecvMessage() < 0 ) return -1;
		system("cls");
		OtherInfoUI(op,1);
		memset(input,0,sizeof(input));
		cout<<"请输入指令：";
		gets_s(input,sizeof(input));
		ToInfo(" 玩家交互页面！");
		if(strcmp(input,"attack") == 0)
		{

			if(Attack(CurrentPlayer,op)<0) return -1;
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"group") == 0)
		{
			ToInfo(" 暂未开发。");
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"exit") == 0)
		{
			cout<<"正在退出..."<<endl;
			break;
		}
		else 
		{
			SendPlayer(CurrentPlayer);
			ToInfo("  非法输入！");
		}
	}
	return 1;
}

int OtherInfoUI(Player p, int funcid)
{
	if(funcid == 0)
	{
		system("cls");
		CutLine("玩家信息");
		char* blank="                           \0";
		unsigned int i = 0;
		cout<<blank<<"@";
		for(i = 0;i < 20; i++)
			cout<<"-";
		cout<<"@"<<endl;
		cout<<blank<<"   该玩家已经下线"<<endl;
		cout<<blank<<"@";
		for(i = 0;i < 20; i++)
			cout<<"-";
		cout<<"@"<<endl;
		CutLine("游戏提示");
		HintPersionalInfo();
		CutLine(0);
		getchar();
	}
	else
	{
		system("cls");
		CutLine("玩家信息");
		DisplayPlayerInfo(p);
		CutLine("游戏提示");
		HintOtherInfo();
		CutLine("游戏信息");
		if(info) cout<<info<<endl;
		else cout<<endl;
		CutLine(0);
	}
	return 1;
}
int DealPlayerInfo(char *buff,Player &p)
{
	string str(buff);

	if(str.find(';',0) == string::npos)
	{
		cout<<"服务端数据异常"<<endl;
		return -1;
	}
	vector<string> info = split(str,';');
	
	if(info.size() < 10)
	{
		cout<<"服务端数据异常"<<endl;
		return -1;
	}
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
int DeleteGoods(char *name)
{
	unsigned int i = 0;
	vector<int> store;
	if(!CurrentPlayer.goods.empty())
		for(i = CurrentPlayer.goods.size()-1; i >= 0; i--)
		{
			if(strcmp(goods[CurrentPlayer.goods[i]].name,name) != 0)
			{
				store.push_back(CurrentPlayer.goods[i]);
				CurrentPlayer.goods.pop_back();
			}else
			{
				CurrentPlayer.goods.pop_back();
				break;
			}
		}
	if(!store.empty())
	{
		for(i = 0; i < store.size(); i++)
		{
			CurrentPlayer.goods.push_back(store[i]);
		}
	}
	return 1;
}
int Attack(Player p1, Player p2)
{
	char buff[128];
	memset(buff, 0, sizeof(buff));
	sprintf_s(buff, "%s", "attack");
	send(sock, buff, strlen(buff), 0);

	send(sock, p2.name, sizeof(p2.name), 0);

	char res[1024];
	memset(res,0,sizeof(res));
	if(recv(sock, res, sizeof(res), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, "the server has error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}
	printf("[OK]\n");

	if(strcmp("noinfo",res) == 0)
	{
		OtherInfoUI(CurrentPlayer,0);
		return 1;
	}

	char input[64];
	ToInfo(" 玩家对战界面！");

	system("cls");
	AttackUI(p1,p2);
	memset(input,0,sizeof(input));

	char num[64];
	memset(num, 0, sizeof(num));
	cout<<"请输入指令：";
	char status[256];
	memset(status, 0, sizeof(status));
	gets_s(input,sizeof(input));
	ToInfo(" 玩家对战页面！");
	if(strcmp(input,"attack") == 0)
	{
		sprintf_s(num,"%d",p1.attack);
		sprintf_s(status,"你对%s进行普通攻击，攻击了%d滴血",p2.name,p1.attack);
		ToInfo(status);
		send(sock, num, sizeof(num), 0);
	}
	else if(strcmp(input,"magic") == 0)
	{
		int mgicnum = p1.attack + 13;
		sprintf_s(num,"%d",p1.attack);
		sprintf_s(status,"你对%s进行魔法攻击，攻击了%d滴血",p2.name,mgicnum);
		ToInfo(status);
		send(sock, num, sizeof(num), 0);
	}
	else if(strcmp(input,"exit") == 0)
	{
		sprintf_s(num,"0");
		ToInfo("逃跑了...");
		send(sock, num, sizeof(num), 0);
	}
	else 
	{
		ToInfo("  非法输入！");
	}
	return 1;
}
void AttackUI(Player p1, Player p2)
{
	CutLine("攻方玩家");
	PersionInfo(p1);
	CutLine(0);
	cout<<"                                        VS"<<endl;
	CutLine(0);
	CutLine("守方玩家");
	PersionInfo(p2);
	CutLine("游戏提示");
	HintAttack();
	CutLine("游戏信息");
	if(info) cout<<info<<endl;
	else cout<<endl;
	CutLine(0);
}



int Defense()
{
	Player attack;
	RecvPlayer(attack);

	char num[64];
	memset(num,0,sizeof(num));
	if(recv(sock, num, sizeof(num), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, "the server has error !\n");
		cout<<"disconnect！"<<endl;
		return -1;
	}

	char res[1024];
	memset(res,0,sizeof(res));
	sprintf_s(res,"%s攻击了你%s滴血!",attack.name,num);
	
	ToInfo(res);

	system("cls");
	AttackUI(attack,CurrentPlayer);
	cout<<"回车键继续...";

	getchar();
	return 1;
}