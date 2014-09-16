#include "func.h"

const char* FileDirectory = "data/";

int Random(int a, int b)
{
	int c=(rand() % (a-b))+ a;
	return c;
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
	}

	for(int i = 0; i< res->height ; i++)
	{
		for(int j = 0; j< res->width ; j++)
			cout<<res->map[i][j]<<" ";
		cout<<endl;
	}
	return res;
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


void ReadStrToFile(const char* file, const char* content, int size)
{
	ofstream out;
	char *s = append(FileDirectory, file);
	out.open(s);
	//out.write((const char *)&content, sizeof(char)*size);
	out<<content;
	out.close();
}