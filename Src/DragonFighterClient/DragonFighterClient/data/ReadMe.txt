DATA1 �汾
DATA2 UI
DATA3 ��ͼ
DATA4 ��Դ
DATA5 ��ϵ
DATA6 ��չ

�� ���������� 1
��.���������� ɳĮ 2
��*���ݵ�  �������� 3
��+��Ĺ�� ���Խ��� 4
��-����·  �������� 5
'#'  ���� ���Խ��� 6
��^��ɽ�� ������ 7
��=������ ������ 8
 Ц�� �Լ� 10
��@�� ��� 9

/*
    function id
	1 ��Ѫ
	2 ��ħ
	3 �ӹ�����
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
		cout<<"������ָ�";
		gets_s(input,sizeof(input));
		ToInfo(" ��ҽ���ҳ�棡");
		if(strcmp(input,"attack") == 0)
		{
			
			if(Attack(CurrentPlayer,op)<0) return -1;
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"group") == 0)
		{
			ToInfo(" ��δ������");
			SendPlayer(CurrentPlayer);
		}
		else if(strcmp(input,"exit") == 0)
		{
			cout<<"�����˳�..."<<endl;
			break;
		}
		else 
		{
			SendPlayer(CurrentPlayer);
			ToInfo("  �Ƿ����룡");
		}
	}