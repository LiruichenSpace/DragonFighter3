#ifndef  FUNC
#define FUNC

#include <string.h>
#include <fstream>
#include<vector>
#include<iostream>
using namespace std;

struct Point
{
	int x, y;
};
struct Player;

struct Goods
{
	int  id;
	char name[64];
	char construct[128];
	int  functionId;
	int  effect;
	int  price;
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
	char password[64];
	int  grade;
	int  isGroup;
	int  permission;
	int  red;
	int  blue;
	Point  position;
	char profession[32];
	int  money;
	int  attack;
	vector<int> goods;
	int  AllEx;
	int  MaxEx;
	vector<int> magics;
	bool isLogin;
};


struct Map 
{
	int width;
	int height;
	int** map;
};

vector<string> split(string str, char symbol);
char *append(const char * str1,const char* str2);

char *ReadFileToStr(const char* file, int buflength);
void WriteStrToFile(const char* file, const char* content, int size = 0);

Map *ReadMapFromFile(const char *file);

int Random(int a, int b);

inline char itoc( int i )
{
	return (char)(48+i);
}

inline int ExCalcu(int grade)
{
	return 2^grade + 20 + 7 * grade;
}
#endif