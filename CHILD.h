#ifndef _CHILD_
#define _CHILD_

#include <windows.h>
#include <cstdio>
#include <ctime>

#define Screen_FullWidth GetSystemMetrics(SM_CXSCREEN)
#define Screen_FullHeight GetSystemMetrics(SM_CYSCREEN)
#define Window_FullWidth 1250
#define Window_FullHeight 630
#define Window_Child_FullWidth 300
#define Window_Child_FullHeight 100

#define Game_Menu 0
#define Game_Ready 1
#define Game_Play 2
#define Game_Pause 3
#define Game_Over 4
#define Game_Boom 5
#define Game_Edit 6

HWND hwnd_child,hwnd_edit,hwnd_button;

int Game_State,Last_Game_State,Game_Level,Score_State;
int Button_State;

int Coin_Count;
int Shell_Time,Boom_Time;
double Shell_Nomorl_Speed;

int background_bTrans,background_clock;

HBITMAP child_button,child_background;

char Window_Text[61];
char* Window_P=NULL;

COLORREF Spare_Color[10] = {RGB(139,69,19),RGB(138,54,36),RGB(210,25,30),RGB(225,125,64),RGB(240,230,140),
							RGB(180,143,143),RGB(199,97,20),RGB(115,74,18),RGB(94,38,18),RGB(244,164,96)};

LRESULT CALLBACK ChildProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_SIZE:
		{
			LONG_PTR Style = ::GetWindowLongPtr(hwnd,GWL_STYLE);
			Style = Style &~WS_CAPTION &~WS_SYSMENU &~WS_SIZEBOX;
			::SetWindowLongPtr(hwnd, GWL_STYLE, Style);
			break;
		}
		
		default: return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}

LRESULT CALLBACK ButtonProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_LBUTTONDOWN:{
			
			GetWindowText(hwnd_edit,Window_P,61);
			
			if(strlen(Window_P)!=0)
			{
				int Window_P_All=0;
				
				for(int i=0;i<strlen(Window_P);i++)
				{
					if(Window_P[i]==32) Window_P_All++; else break;
				}
				
				if(Window_P_All==strlen(Window_P)) return 0;
				
				DestroyWindow(hwnd_edit);
				DestroyWindow(hwnd_button);
				DestroyWindow(hwnd_child);
			
				Game_State=Game_Over; background_clock=clock(); background_bTrans=255; Window_P=NULL;
			}
				
			return 0;
		}
		
		default: return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}

void game_child_create(HWND hwnd_father,WNDCLASSEX wndc,int cmdShow)
{
	wndc.lpfnWndProc = ChildProc;
	wndc.hIcon = NULL;
	wndc.hCursor = NULL;
	wndc.lpszClassName = "WindowClass_Child";
	wndc.hbrBackground = CreatePatternBrush(child_background);
	
	RegisterClassEx(&wndc);
	
	hwnd_child= CreateWindowEx(0,"WindowClass_Child", "",WS_BORDER|WS_OVERLAPPEDWINDOW^WS_SYSMENU^WS_CAPTION^WS_THICKFRAME,
				(Screen_FullWidth-Window_Child_FullWidth)/2,(Screen_FullHeight-Window_Child_FullHeight)/2, 
				Window_Child_FullWidth,Window_Child_FullHeight,NULL,NULL,NULL,NULL);
	
	if (hwnd_child==NULL)
	{
		MessageBox(NULL, "Window Creation Failed!","Error!", MB_OK); Game_State=Last_Game_State;
		return;
	}
	
	ShowWindow(hwnd_child, cmdShow);
	UpdateWindow(hwnd_child);
	
	SetWindowPos(hwnd_child,HWND_TOP,(Screen_FullWidth-Window_Child_FullWidth)/2,(Screen_FullHeight-Window_Child_FullHeight)/2, 
				Window_Child_FullWidth,Window_Child_FullHeight,SWP_SHOWWINDOW);
	
	wndc.lpfnWndProc = ButtonProc;
	wndc.hIcon = NULL;
	wndc.hCursor = NULL;
	wndc.lpszClassName = "WindowClass_Button";
	wndc.hbrBackground = CreatePatternBrush(child_button);
	
	RegisterClassEx(&wndc);
	
	hwnd_button= CreateWindowEx(0,"WindowClass_Button","",WS_CHILD|WS_OVERLAPPEDWINDOW^WS_SYSMENU^WS_CAPTION^WS_THICKFRAME,
				(Window_Child_FullWidth-80)/2,70, 80,26,hwnd_child,NULL,NULL,NULL);
	
	if (hwnd_button==NULL)
	{
		MessageBox(NULL, "Button Creation Failed!","Error!", MB_OK); DestroyWindow(hwnd_child); Game_State=Game_Over;
		return;
	}
	
	ShowWindow(hwnd_button, cmdShow);
	UpdateWindow(hwnd_button);
	
	hwnd_edit=CreateWindow("edit",NULL,WS_CHILD|WS_VISIBLE|WS_BORDER,(Window_Child_FullWidth-100)/2,(Window_Child_FullHeight-21)/2,100,21,hwnd_child,NULL,NULL,NULL);
	SetFocus(hwnd_edit);
	
	return;
}
#endif
