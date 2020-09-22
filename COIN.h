#ifndef _COIN_
#define _COIN_

#include <windows.h>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <cstring>

#define Coin_Exist 0
#define Coin_Disappear 1

#define Penguin_Retreat -1
#define Penguin_Forward 1

#define Shell_Unfire 0
#define Shell_Fire 1

#define Button_Up 0
#define Button_Down 1

#define number_swap(Number1,Number2) {int Number_Change=Number1; Number1=Number2; Number2=Number_Change;}

void DrawTransBitmap(HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidthDest,int nHeightDest,
					HDC hdcSrc,int nXOriginSrc,int nYOriginSrc,COLORREF crTransparent);

void RotateAnyAngle(HDC hdcDest,int placex,int placey,int SrcWidth,int SrcHeight,
					HDC hdcSrc,int nXOriginSrc,int nYOriginSrc,COLORREF crTransparent,double angle,bool ask=true);
					
int Hit_Check(HWND hwnd,double check1x,double check1y,double check1w,double check1h,double check2x,double check2y,double check2w,double check2h,
				double angle=0,double frequency=1,double extrax=0,double extray=0);

void Desalination_enlarge(HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidthDest,int nHeightDest,
							HDC hdcSrc,int nXOriginSrc,int nYOriginSrc,double multiple,BLENDFUNCTION bf,COLORREF crTransparent,bool ask=true);

void char_print(HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidthDest,int nHeightDest,HFONT hf,const char* wanna_char);

void char_swap(char* ch1,char* ch2);

struct PENGUIN
{
	HBITMAP PBmp;
	int State;
	int middle_place_x,middle_place_y;
	int place_x,place_y;
	double Angle;
};

struct SHELL
{
	int place_x,place_y;
	int State;
	
	double speed;
	double Angle;
};

struct BOOM
{
	int Now_Radius,Max_Radius;
	int middle_place_x,middle_place_y;
	
	int bTrans;
	COLORREF color;
	BLENDFUNCTION bf;
	
	int Time_Arrangement;
};

void Boom_Paint(HDC hdcDest,BOOM P_B);

class COIN
{
	public:
		
		BLENDFUNCTION bf;
		int bTrans,Coin_x,Coin_y,Coin_State;
		double multiple;
		
		COIN();
		
		void Coin_Print(HDC hdcDest,HDC hdcSrc,HBITMAP Coin_Bitmap);
		void Coin_Compute();
		void Coin_Reset(int Coinx,int Coiny);
};

#endif
