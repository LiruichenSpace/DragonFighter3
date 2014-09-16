DATA1 版本
DATA2 UI
DATA3 地图
DATA4 资源
DATA5 关系
DATA6 扩展

‘ ’可以行走 1
‘.’可以行走 沙漠 2
‘*’草地  可以行走 3
‘+’墓地 可以进入 4
‘-’道路  可以行走 5
'#'  村落 可以进行 6
‘^’山峰 不可走 7
‘=’河流 不可走 8
 笑脸 自己 10
‘@’ 玩家 9

/*
    function id
	1 加血
	2 加魔
	3 加攻击力
*/ 

/*
$
,,,,%
,,,,%
$
*/
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