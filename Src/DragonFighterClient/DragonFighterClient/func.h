#ifndef  FUNC
#define FUNC

#include <string.h>
#include <fstream>
#include<vector>
#include<iostream>
using namespace std;

struct Point;
struct Player;
struct NPC;
struct Goods;
struct Map;
struct Magic;
struct Place;
struct GroupPlayer;
struct OnlinePlayer;

struct Point
{
	int x, y;
	bool operator ==(Point p);
	Point operator =(Point p);
};

struct OnlinePlayer
{
	Point pos;
	char name[128];
	int  isGroup;
};


struct GroupPlayer
{
	OnlinePlayer baseinfo;
	int  red;
	int  blue;
};

struct Player
{
	char name[32];
	int  grade;
	bool isGroup;
	bool permission;
	int  red;
	int  blue;
	Point  position;
	char profession[32];
	int  money;
	int  attack;
	vector<int> goods;
	int  AllEx;
	int  MaxEx;
	vector<int> magic;
	GroupPlayer teammate;
};

struct Map 
{
	int width;
	int height;
	int** map;
};

inline void ShowPlayer()
{
	int a;
	a=1;
	putchar(a);
}

struct NPC
{
	int  id;
	char name[64];
	char speak[256];
};

inline int ExCalcu(int grade)
{
	return 2^grade + 20 + 7 * grade;
}

struct Goods
{
	int  id;
	char name[64];
	char construct[128];
	int  functionId;
	int  effect;
	int  price;
};

struct Magic
{
	int id;
	char name[64];
	char construct[256];
	int functionid;
	int effect;
	int grade;
};

struct Place
{
	int id;
	char name[64];
	char construct[1024];
	vector<NPC> NPC;
	vector<Goods> goods;
	Point pos;
};

struct MapBuf
{
	int x, y, index;
};

inline void CityMenu()
{
	cout<<"===buy.购买道具—use.使用道具—hotel.旅店住宿—info.个人信息—exit.离开城镇==="<<endl;
}

//int DeleteVector(Vector<>)

vector<string> split(string str, char symbol);
char *append(const char * str1,const char* str2);

char *ReadFileToStr(const char* file, int buflength);
void WriteStrToFile(const char* file, const char* content, int size = 0);
Map *ReadMapFromFile(const char *file);

void DisplayMap(Map *map, vector<string> symbol, Point p);

inline char itoc( int i )
{
	return (char)(48+i);
}

void PersionInfo(Player p);

inline void HintPersionalInfo()
{
	cout<<"按任意键退出！"<<endl;
}



inline void HintOtherInfo()
{
	cout<<" group：组队  attack：攻击  exit：退出"<<endl;
}

inline void HintGoods()
{
	cout<<"use：使用道具  discard：丢弃道具  exit：退出道具界面"<<endl;
}

inline void HintAttack()
{
	cout<<"use：使用道具  attack：攻击 magic：魔法  exit：逃跑"<<endl;
}


inline void pause()
{
	getchar();
	getchar();
}



int Random(int a, int b);

void DisplayGoods(vector<Goods> g,vector<int> i);

void DisplayPlayerInfo(Player p);

#endif