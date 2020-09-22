#include <windows.h>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <cstring>
#include "COIN.h"
#include "CHILD.h"

HWND hwnd_main;
WNDCLASSEX wc;
int nCmdShow;
FILE *fp;

HDC t_hdc,g_hdc,m_hdc;
HBITMAP double_buffer_background,background[2];
BLENDFUNCTION background_bf;
HFONT hfont;

HBITMAP Coin,Title,Battery;
HBITMAP button[3];
HBITMAP Shell;

COIN coin[30];
PENGUIN penguin;
SHELL shell[10];
BOOM boom[20];

char high_score[20][100];
char string[25];

void game_Init_Data(HWND hwnd);
void game_Init_Bmp(HWND hwnd);
void game_Play(HWND hwnd);
void game_check(HWND hwnd);
void game_Paint(HWND hwnd);
void game_reset(HWND hwnd);
void game_Clear(HWND hwnd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	::nCmdShow = nCmdShow;
	MSG msg;
	
	game_Init_Data(hwnd_main);
	
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc;
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	wc.hbrBackground = NULL;
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(hInstance,"DICO");
	wc.hIconSm		 = LoadIcon(hInstance,"DICO");
	
	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd_main = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Corsairs",WS_VISIBLE|WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MINIMIZEBOX^WS_MAXIMIZEBOX,
	       (Screen_FullWidth-Window_FullWidth)/2,(Screen_FullHeight-Window_FullHeight)/2,Window_FullWidth,Window_FullHeight,NULL,NULL,hInstance,NULL);

	if(hwnd_main == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	game_Init_Bmp(hwnd_main);
	
    ShowWindow(hwnd_main,nCmdShow);
    UpdateWindow(hwnd_main); 
    
    srand(time(NULL));
    	
	game_reset(hwnd_main);
    
	while (msg.message != WM_QUIT)  {
	if (PeekMessage(&msg,0,0,0,PM_REMOVE))
        {  
            TranslateMessage(&msg);
            DispatchMessage(&msg); 
        }  
        else  
        {
        	int start_clock = clock();
        	
			game_Play(hwnd_main);
        	game_Paint(hwnd_main);
        	if(Game_State == Game_Play) game_check(hwnd_main);
        	
        	Button_State=Button_Up;
			
        	while(clock()-start_clock<30);
        }
    }
	return msg.wParam;
}

void game_Init_Data(HWND hwnd)
{
	fp=fopen("res\\scorerecord","rb");
	for(int i=0;i<20;i++) fread(high_score[i],100,1,fp);
	fclose(fp);
	
	for(int i=0;i<20;i++)
	{
		boom[i].bf.BlendOp = AC_SRC_OVER; 
        boom[i].bf.BlendFlags = 0;
        boom[i].bf.AlphaFormat = 0;
	}
	
	Shell_Nomorl_Speed=10;
		
	background_bf.BlendOp              = AC_SRC_OVER;
    background_bf.BlendFlags           = 0;
    background_bf.SourceConstantAlpha  = background_bTrans;
    background_bf.AlphaFormat          = 0;
}

void game_Init_Bmp(HWND hwnd)
{
	HINSTANCE hInstance;
	
	t_hdc = GetDC(hwnd);
	g_hdc = CreateCompatibleDC(t_hdc);
	m_hdc = CreateCompatibleDC(g_hdc);
	
	double_buffer_background = CreateCompatibleBitmap(t_hdc,Window_FullWidth,Window_FullHeight);
	
	hfont = CreateFont(30,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,"Comic Sans MS");
	
	Coin = LoadBitmap(hInstance,"COIN");
	Title = LoadBitmap(hInstance,"TITLE");
	penguin.PBmp = LoadBitmap(hInstance,"PENGUIN");
	Battery = LoadBitmap(hInstance,"BATTERY");
	Shell = LoadBitmap(hInstance,"SHELL");
	
	background[0] = LoadBitmap(hInstance,"BK0");
	background[1] = LoadBitmap(hInstance,"BK1");
	
	button[0] = LoadBitmap(hInstance,"BUTTON0");
	button[1] = LoadBitmap(hInstance,"BUTTON1");
	button[2] = LoadBitmap(hInstance,"BUTTON2");
	
	child_button = LoadBitmap(hInstance,"BUTTON3");
	child_background = LoadBitmap(hInstance,"BK2");
}

void game_Paint(HWND hwnd)
{
	SelectObject(g_hdc,double_buffer_background);
	
	SelectObject(m_hdc,background[0]);
	BitBlt(g_hdc,0,0,1250,630,m_hdc,0,0,SRCCOPY);
	
	if(Game_State == Game_Menu)
	{
		SelectObject(m_hdc,Title);
		DrawTransBitmap(g_hdc,250,90,382,90,m_hdc,0,0,RGB(255,255,255));
	}
	
	if(Game_State == Game_Ready||Game_State == Game_Play||(Game_State==Game_Boom&&clock()-Boom_Time<=3000))
	{
		char_print(g_hdc,10,10,55,25,hfont,"Level"); char_print(g_hdc,70,10,50,25,hfont,itoa(Game_Level,string,10));
		
		if(Coin_Count==0) char_print(g_hdc,845,10,13,25,hfont,itoa(0,string,10));
		else
		{
			int Coin_num=0,cc=Coin_Count*10;
			
			while(cc/10>0)
			{
				Coin_num++; cc=cc/10;
			}
			
			char_print(g_hdc,858-13*Coin_num,10,13*Coin_num,25,hfont,itoa(Coin_Count,string,10));
		}
		
		SelectObject(m_hdc,Battery);
		DrawTransBitmap(g_hdc,185,55,520,520,m_hdc,0,0,RGB(255,255,255));
		
		for(int i=0;i<30;i++) coin[i].Coin_Print(g_hdc,m_hdc,Coin);
		
		for(int i=0;i<10;i++) if(shell[i].State == Shell_Fire)
		{
			SelectObject(m_hdc,Shell);
			DrawTransBitmap(g_hdc,shell[i].place_x,shell[i].place_y,25,25,m_hdc,0,0,RGB(255,255,255));
		}
		
		if(Game_State == Game_Ready||Game_State == Game_Play||(Game_State==Game_Boom&&clock()-Boom_Time<=1500))
		{
			SelectObject(m_hdc,penguin.PBmp);
			RotateAnyAngle(g_hdc,penguin.place_x,penguin.place_y,50,50,m_hdc,0,0,RGB(255,255,255),penguin.Angle+M_PI/2*(penguin.State-1));
		}
		
		
		if(Game_State==Game_Boom&&clock()-Boom_Time<=3000) for(int i=0;i<20;i++) if(clock()-Boom_Time>boom[i].Time_Arrangement) Boom_Paint(g_hdc,boom[i]);
	}
	
	if(Game_State==Game_Menu||Game_State==Game_Over)
	{
		for(int i=0;i<20;i+=2)
		{
			if(i<10)
			{
				char_print(g_hdc,70,270+i*25,200,30,hfont,high_score[i]);
				char_print(g_hdc,300,270+i*25,200,30,hfont,high_score[i+1]);
			}
			
			else
			{
				char_print(g_hdc,520,270+(i-10)*25,200,30,hfont,high_score[i]);
				char_print(g_hdc,750,270+(i-10)*25,200,30,hfont,high_score[i+1]);
			}
		}
	}
	
	if(Game_State==Game_Over)
	{
		char_print(g_hdc,350,100,130,25,hfont,"You Scored"); coin[15].Coin_Print(g_hdc,m_hdc,Coin);
		
		if(Coin_Count==0) char_print(g_hdc,410,143,13,25,hfont,itoa(0,string,10));
		else
		{
			int Coin_num=0,cc=Coin_Count*10;
			
			while(cc/10>0)
			{
				Coin_num++; cc=cc/10;
			}
			
			char_print(g_hdc,423-13*Coin_num,143,13*Coin_num,25,hfont,itoa(Coin_Count,string,10));
		}
	}	
		
	SelectObject(m_hdc,background[1]);
	AlphaBlend(g_hdc,0,0,1250,630,m_hdc,0,0,1250,630,background_bf);
	
	SelectObject(m_hdc,background[1]);
	BitBlt(g_hdc,900,0,350,630,m_hdc,0,0,SRCCOPY);
	
	if(Game_State == Game_Ready||Game_State == Game_Menu) SelectObject(m_hdc,button[0]);
	if(Game_State == Game_Play ||Game_State == Game_Boom) SelectObject(m_hdc,button[1]);
	if(Game_State == Game_Over) SelectObject(m_hdc,button[2]); 
	
	if(Button_State==Button_Up) BitBlt(g_hdc,975,215,200,200,m_hdc,0,0,SRCCOPY);
	else StretchBlt(g_hdc,980,220,190,190,m_hdc,0,0,200,200,SRCCOPY);
	
	BitBlt(t_hdc,0,0,Window_FullWidth,Window_FullHeight,g_hdc,0,0,SRCCOPY); 
}

void game_Play(HWND hwnd)
{
	if(Button_State==Button_Down)
	{
		if(Game_State==Game_Play) penguin.State=-penguin.State;
		
		if(Game_State==Game_Ready) Game_State=Game_Play;
		
		if(Game_State==Game_Menu)
		{
			Game_State=Game_Ready; Game_Level=1; Coin_Count=0; game_reset(hwnd);
		}
		
		if(Game_State==Game_Over)
		{
			Game_State=Game_Ready; Game_Level=1; Coin_Count=0; game_reset(hwnd); background_clock=clock(); background_bTrans=255; Shell_Nomorl_Speed=10;
		}
	}
	
	if(Game_State == Game_Play || (Game_State==Game_Boom && clock()-Boom_Time<=3000))
	{
		if(Game_State == Game_Play) penguin.Angle+=0.05*penguin.State;
		
		while(penguin.Angle>M_PI*2) penguin.Angle-=M_PI*2;
		while(penguin.Angle<M_PI*(-2)) penguin.Angle+=M_PI*2;
		
		penguin.middle_place_x = 415+257*sin(penguin.Angle);
		penguin.middle_place_y = 287-257*cos(penguin.Angle);
		
		if(cos(penguin.Angle)==1)
		{
			penguin.middle_place_x = 440;
			penguin.middle_place_y = 55;
		}
		
		penguin.place_x = penguin.middle_place_x-25*1.0*(abs(sin(penguin.Angle))+abs(cos(penguin.Angle)));
		penguin.place_y = penguin.middle_place_y-25*1.0*(abs(sin(penguin.Angle))+abs(cos(penguin.Angle)));
		
		if(clock()-Shell_Time>500-Game_Level*30 && Game_State == Game_Play) for(int i=0;i<10;i++) if(shell[i].State == Shell_Unfire)
		{
			shell[i].place_x = 445-13; shell[i].place_y = 315-13;
			
			shell[i].speed = Shell_Nomorl_Speed;
			
			double arrive_time = 260/shell[i].speed; int AI_Catch=rand()%4-1;
			
			if(AI_Catch == 2) shell[i].Angle = penguin.Angle+0.05*penguin.State*arrive_time;
			else shell[i].Angle = penguin.Angle+0.03*AI_Catch*penguin.State*arrive_time;
			
			shell[i].State = Shell_Fire; Shell_Time=clock(); break;
		}
		
		for(int i=0;i<10;i++) if(shell[i].State == Shell_Fire)
		{
			shell[i].place_x+= shell[i].speed*sin(shell[i].Angle);
			shell[i].place_y-= shell[i].speed*cos(shell[i].Angle);
			
			if(shell[i].place_x<0||shell[i].place_x>900||shell[i].place_y<0||shell[i].place_y>630) shell[i].State = Shell_Unfire;
		}
		
		int Disappear_Count=0;
		
		for(int i=0;i<30;i++) if(coin[i].Coin_State == Coin_Disappear)
		{
			if(coin[i].bTrans>0)
			{
				coin[i].bTrans-=15; coin[i].bf.SourceConstantAlpha  = coin[i].bTrans;
			}
			
			if(coin[i].multiple<2) coin[i].multiple+=0.048; else coin[i].multiple=2;
			
			Disappear_Count++;
		}
		
		if(Disappear_Count==29)
		{
			Game_Level++; Game_State=Game_Ready; Coin_Count+=11; Shell_Nomorl_Speed+=0.4; game_reset(hwnd); 
		}
		
		if(Game_State==Game_Boom) for(int i=0;i<20;i++) if(clock()-Boom_Time>boom[i].Time_Arrangement)
		{
			if(boom[i].Now_Radius < boom[i].Max_Radius) boom[i].Now_Radius += 2;
			else if(boom[i].bTrans>=10)
			{
				boom[i].bTrans-=10; boom[i].bf.SourceConstantAlpha = boom[i].bTrans;
			}
		}
	}
	
	if(Game_State==Game_Boom && clock()-Boom_Time>3000)
	{
		Game_State = Game_Over; background_clock=clock(); background_bTrans=255;
	}
	
	if(Game_State==Game_Over)
	{	
		int score[10];
		
		for(int i=1;i<20;i+=2) score[(i-1)/2]=atoi(high_score[i]);
		
		if(Score_State==0&&Coin_Count>=70)
		{
			for(int i=0;i<10;i++)
			{
				if(Coin_Count>score[i])
				{
					for(int j=10;j>i;j--)
					{
						strcpy(high_score[j*2+1],high_score[j*2-1]); strcpy(high_score[j*2],high_score[j*2-2]);
					}
				
					strcpy(high_score[i*2+1],itoa(Coin_Count,string,10)); Window_P=high_score[i*2];
				
					Game_State=Game_Edit; background_bTrans=255; game_child_create(hwnd_main,wc,nCmdShow); Score_State=1; break;
				}
			}
		}
		
		coin[15].Coin_Reset(430,145);
	}
	
	if(Game_State==Game_Edit&&GetAsyncKeyState(VK_LBUTTON)) SetWindowPos(hwnd_child,HWND_TOP,(Screen_FullWidth-Window_Child_FullWidth)/2,(Screen_FullHeight-Window_Child_FullHeight)/2, 
															Window_Child_FullWidth,Window_Child_FullHeight,SWP_SHOWWINDOW);
	
	if(clock()-background_clock>1000&&background_bTrans>=10&&Game_State!=Game_Edit) background_bTrans-=10;
	
	background_bf.SourceConstantAlpha = background_bTrans;
	
}

void game_check(HWND hwnd)
{
	for(int i=0;i<30;i++) if(coin[i].Coin_State!=Coin_Disappear)
	{
		if((coin[i].Coin_x-penguin.middle_place_x)*(coin[i].Coin_x-penguin.middle_place_x)+
			(coin[i].Coin_y-penguin.middle_place_y)*(coin[i].Coin_y-penguin.middle_place_y)<=1370)
		{
			coin[i].Coin_State=Coin_Disappear; Coin_Count++; PlaySound("res\\click.wav",NULL,SND_FILENAME|SND_ASYNC);
		}
	}
	
	for(int i=0;i<10;i++) if(shell[i].State == Shell_Fire)
	{
		if((shell[i].place_x-penguin.middle_place_x)*(shell[i].place_x-penguin.middle_place_x)+
			(shell[i].place_y-penguin.middle_place_y)*(shell[i].place_y-penguin.middle_place_y)<=1370)
		{
			Game_State = Game_Boom; Boom_Time = clock(); shell[i].State = Shell_Unfire; PlaySound("res\\boom.wav",NULL,SND_FILENAME|SND_ASYNC);
			
			if(abs(penguin.Angle-shell[i].Angle)>0.05) penguin.Angle+=0.05*penguin.State;
			
			else penguin.Angle+=abs(penguin.Angle-shell[i].Angle)*penguin.State;
			
			penguin.middle_place_x = 415+257*sin(penguin.Angle);
			penguin.middle_place_y = 287-257*cos(penguin.Angle);
			
			for(int I=0;I<20;I++)
			{
				boom[I].middle_place_x = penguin.middle_place_x+rand()%30+15;
				boom[I].middle_place_y = penguin.middle_place_y+rand()%30+15;
				boom[I].bTrans = 255;
				boom[I].bf.SourceConstantAlpha = boom[I].bTrans;
			}
				
			return;
		}
	}
}

void game_reset(HWND hwnd)
{
	background_bTrans=255;
	
	penguin.Angle = 0;
	penguin.State = Penguin_Forward;
	penguin.middle_place_x = 415;
	penguin.middle_place_y = 30;
	penguin.place_x = 415;
	penguin.place_y = 30;
	
	Score_State=0;
	
	for(int i=1;i<15;i++)
	{
		if(i<11)
		{
			shell[i-1].State = Shell_Unfire;
		}
		
		coin[i].Coin_Reset(445-257*sin(M_PI/15*i)-12,315-257*cos(M_PI/15*i)-12);
		coin[i+15].Coin_Reset(445+257*sin(M_PI/15*i)-12,315-257*cos(M_PI/15*i)-12);
	}
	
	for(int i=0;i<20;i++)
	{
		boom[i].color = Spare_Color[rand()%10];
		boom[i].Max_Radius = 35+rand()%20;
		boom[i].Now_Radius = 0;
		boom[i].bTrans = 255;
		boom[i].bf.SourceConstantAlpha = boom[i].bTrans;
		boom[i].Time_Arrangement = rand()%1440+300;
	}
	
	coin[0].Coin_Reset(445-12,315+257-12);
	
	coin[15].Coin_Reset(865,13);
}

void game_Clear(HWND hwnd) 
{
	ReleaseDC(hwnd,t_hdc);
	DeleteObject(g_hdc);
	DeleteObject(m_hdc);
	DeleteObject(double_buffer_background);
	DeleteObject(hfont);
	
	DeleteObject(Coin);
	DeleteObject(Title);
	for(int i=0;i<3;i++)
	{
		if(i<2) DeleteObject(background[i]);
		DeleteObject(button[i]);
	}
	DeleteObject(child_button);
	DeleteObject(child_background);
	
	int Window_P_Al=0;
				
	if(Window_P!=NULL)
	{
		for(int i=0;i<strlen(Window_P);i++)
		{
			if(Window_P[i]==32) Window_P_Al++; else break;
		}

		if(Window_P_Al==strlen(Window_P)||strlen(Window_P)==0) strcpy(Window_P,"No Name");
	}
	
	fp=fopen("res\\scorerecord","wb");
	for(int i=0;i<20;i++) fwrite(high_score[i],100,1,fp);
	fclose(fp); 
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	
	switch(Message) {
		
		case WM_LBUTTONDOWN:{
			
			POINT p; p.x=LOWORD(lParam); p.y=HIWORD(lParam);
			
			if((p.x-1075)*(p.x-1075)+(p.y-315)*(p.y-315)<=10000 && background_bTrans<=10 && Game_State!= Game_Edit) Button_State = Button_Down;
			
			break;
		}
		
		case WM_LBUTTONUP: if(Button_State == Button_Down) Button_State = Button_Up; break;
        	
		case WM_DESTROY: {
			game_Clear(hwnd);
			PostQuitMessage(0);
			break;
		}		
			
		default: break;
			
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);
}
