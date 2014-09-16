#include "func.h"

const char* FileDirectory = "data/";
bool Point::operator ==(Point p)
{
	return x==p.x&&y==p.y;
}
Point Point::operator =(Point p)
{
	x = p.x;
	y = p.y;
	return *this;
}
int Random(int a, int b)
{
	int c=(rand() % (a-b))+ a;
	return c;
}
void PersionInfo(Player p)
{
	int r = p.red/10;
	int b = p.blue/10;
	cout<<"血量：";
	for (int i = 0; i< 10; i++)
	{
		if(i <= r) cout<<"||";
		else cout<<"  ";
	}
	cout<<" :"<<p.red;
	cout<<"        姓名："<<p.name;
	cout<<"       等级："<<p.grade<<endl;
	
	cout<<"魔力：";
	for (int i = 0; i< 10; i++)
	{
		if(i <= b) cout<<"||";
		else cout<<"  ";
	}
	cout<<" :"<<p.blue;
	cout<<"	       职业："<<p.profession;
	cout<<"       金钱："<<p.money<<endl;

}




char *append(const char * str1,const char* str2)
{
	int length = strlen(str1)+strlen(str2);
	char *res = (char*)malloc(strlen(str1)+strlen(str2));
	unsigned int i =0 ,j = 0; 
	for (; i<strlen(str1); i++)
	{
		res[i] = str1[i];
	}
	for (; j < strlen(str2); j++)
	{
		res[i+j] = str2[j];
	}
	res[i+j] = '\0';
	return res;
}


vector<string> split(string str, char symbol)
{
	vector<string> pool;
	int startpos = 0;
	int endpos = 0;
	while( (endpos = str.find(symbol, endpos)) != -1)
	{
		string s = str.substr(startpos,endpos - startpos);
		endpos++;
		startpos = endpos;
		pool.push_back(s);
	}
	int len;
	if((len = str.size()-startpos)!= 0)
	{
		string endstr = str.substr(startpos, len);
		pool.push_back(endstr);
	}
	return pool;
}

char *ReadFileToStr(const char* file, int buflength)
{
	char *s = append(FileDirectory, file);
	char *buf = (char *)malloc(buflength);
	memset(buf,0,sizeof(char)*buflength);
	ifstream in(s); 
	if(!in)
	{
		cout<<"The file:"<<s<<" is not exist!!"<<endl;
		return 0;
	}
	in.read((char *)buf,sizeof(char)*buflength);
	buf[buflength]='\0';
	in.close();
	return buf;
}


void WriteStrToFile(const char* file, const char* content, int size)
{
	ofstream out;
	char *s = append(FileDirectory, file);
	out.open(s);
	out<<content;
	//out.write((const char *)&content, sizeof(char)*size);
	out.close();
}

void DisplayMap(Map *map, vector<string> symbol, Point p)
{
	char* blank="                           \0";

	int W = 21;
	int H = 11;

	int x = map->width;
	int y = map->height;
	int i = 0, j = 0;
	
	if(W > x) W = x;
	if(H > y) H = y;

	int startx = 0, starty = 0;
	int endx = 0, endy = 0;

	if(p.x < (W - 1)/2) 
	{
		startx = 0;
		endx = startx + W;
	}
	else if((p.x + (W - 1)/2) >= x) 
	{
		endx = x - 1;
		startx = endx - W;
	}else
	{
		startx = p.x - (W - 1)/2;
		endx = p.x + (W - 1)/2;
	}

	if(p.y < (H - 1)/2) 
	{
		starty = 0;
		endy = starty + H;
	}
	else if((p.y + (H - 1)/2) >= y) 
	{
		endy = y - 1;
		starty = endy - H;
	}else
	{
		starty = p.y - (H - 1)/2;
		endy = p.y + (H - 1)/2;
	}
	
	/*cout<<"W: "<<W<<"  H:"<<H<<endl;
	cout<<"startx : "<<startx<<"  starty : "<<starty<<endl;
	cout<<"endx : "<<endx<<"  endy : "<<endy<<endl;
	*/

	cout<<blank<<"+";
	for(i = startx;i < endx; i++)
		cout<<"=";
	cout<<"+"<<endl;
	int index = -1;
	for(j=starty; j<endy; j++)
	{
		cout<<blank<<"|";
		for(i = startx; i < endx; i++)
		{
			index = map->map[j][i];
			if(index == 10) ShowPlayer();
			else cout<<symbol[index].c_str();
		}
		cout<<"|"<<endl;
	}
	cout<<blank<<"+";
	for(i = startx;i < endx; i++)
		cout<<"=";
	cout<<"+"<<endl;
}

void DisplayGoods(vector<Goods> g,vector<int> ints)
{
	char* blank="                           \0";
	unsigned int i = 0;
	cout<<blank<<"@";
	for(i = 0;i < 20; i++)
		cout<<"-";
	cout<<"@"<<endl;

	if(ints.empty()) cout<<blank<<"      没有物品"<<endl;
	else
	{
		for(i = 0; i < ints.size(); i++)
		{
			cout<<blank<<i+1<<"."<<g[ints[i]].name<<endl;
		}
	}
	cout<<blank<<"@";
	for(i = 0;i < 20; i++)
		cout<<"-";
	cout<<"@"<<endl;

}
void DisplayPlayerInfo(Player p)
{
	char* blank="                           \0";
	unsigned int i = 0;
	cout<<blank<<"@";
	for(i = 0;i < 20; i++)
		cout<<"-";
	cout<<"@"<<endl;

	cout<<blank<<"*姓名："<<p.name<<endl;
	cout<<blank<<"*等级："<<p.grade<<endl;
	cout<<blank<<"*血量："<<p.red<<endl;
	cout<<blank<<"*魔力："<<p.blue<<endl;
	cout<<blank<<"*攻击："<<p.attack<<endl;
	cout<<blank<<"*职业："<<p.profession<<endl;
	cout<<blank<<"*金钱："<<p.money<<endl;
	cout<<blank<<"*经验："<<p.AllEx<<endl;
	cout<<blank<<"*升级还需经验："<<p.MaxEx - p.AllEx<<endl;
	cout<<blank<<"*队友：";
	if(!p.isGroup) cout<<"无队友"<<endl;
	else cout<<p.teammate.baseinfo.name<<endl;

	cout<<blank<<"@";
	for(i = 0;i < 20; i++)
		cout<<"-";
	cout<<"@"<<endl;
}

Map* ReadMapFromFile(const char *file)
{
	Map *res = (Map *)malloc(sizeof(Map));
	char *s = ReadFileToStr(file, 2048);
	string buf(s);
	vector<string> info = split(s,',');
	res->width =  atoi(info[0].c_str());
	res->height =  atoi(info[1].c_str());

	res->map = (int**)malloc(sizeof(int) * res->height);
	for(int i = 0; i< res->height ; i++)
	{
		res->map[i] = (int *)malloc(sizeof(int) * res->width);
	}

	for(unsigned int j = 2; j < info.size(); j++)
	{
		int col = (j - 2)/res->width;
		int row = (j - 2)%res->width;
		res->map[col][row] = atoi(info[j].c_str());
		//cout<<"col:"<<col<<" row:"<<row<<" index: "<<res->map[col][row]<<endl;
	}

	/*for(int i = 0; i< res->height ; i++)
	{
		for(int j = 0; j< res->width ; j++)
			cout<<res->map[i][j]<<" ";
		cout<<endl;
	}*/
	return res;
}
