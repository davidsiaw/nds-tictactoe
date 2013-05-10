#include <nds.h>
#include <stdio.h>

void ClearScreen(void)
{
    int i;
     
    for(i = 0; i < 256 * 192; i++)
	{
		VRAM_A[i] = RGB15(0,0,0);
	}
	
	// draw vertical lines
    for(i = 6; i < 186; i++)
	{
		VRAM_A[256 * i + 28 + 10 + 59] = RGB15(31,0,0);
		VRAM_A[256 * i + 28 + 10 + 60] = RGB15(31,0,0);
		
		VRAM_A[256 * i + 28 + 10 + 60 + 59] = RGB15(31,0,0);
		VRAM_A[256 * i + 28 + 10 + 60 + 60] = RGB15(31,0,0);
	}
	
	// draw horizontal lines
    for(i = 28 + 10; i < 28 + 10 + 180; i++)
	{
		VRAM_A[256 * (6+59) + i] = RGB15(31,0,0);
		VRAM_A[256 * (6+60) + i] = RGB15(31,0,0);
		
		VRAM_A[256 * (6+60+59) + i] = RGB15(31,0,0);
		VRAM_A[256 * (6+60+60) + i] = RGB15(31,0,0);
	}
}


#define setPixel(x, y, color) VRAM_A[(x) + (y) * 256] = color

void Line( int x1, int y1, int x2, int y2, int c ) {
    bool swap = abs( y2 - y1 ) > abs ( x2 - x1 );
    int x1t =  swap ? y1 : x1;
    int y1t =  swap ? x1 : y1;
    int x2t =  swap ? y2 : x2;
    int y2t =  swap ? x2 : y2;

    int xs =  x1t < x2t ? x1t : x2t;
    int ys =  x1t < x2t ? y1t : y2t;
    int xt =  x1t < x2t ? x2t : x1t;
    int yt =  x1t < x2t ? y2t : y1t;

    int dx = xt - xs;
    int dy = abs(yt - ys);

    int dT = 2 * ( dy - dx );
    int dS = 2 * dy;
    int d = 2 * dy - dx;
    int x = xs;
    int y = ys;

    if ( swap )  {
        setPixel( y, x, c );
    } else {
        setPixel( x, y, c );
    }

    while ( x < xt ) {
        x++;
        if ( d < 0 ) {
            d = d + dS;
        } else {
            if ( ys < yt ) {
                y++;
            } else {
                y--;
            }
            d = d + dT;
        }
        if ( swap )  {
            setPixel( y, x, c );
        } else {
            setPixel( x, y, c );
        }
    }
}

void Circle( int mx, int my, int r, int color ) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (x <= y) {
        setPixel( mx  + x, my + y, color );
        setPixel( mx  - x, my + y, color );
        setPixel( mx  - x, my - y, color );
        setPixel( mx  + x, my - y, color );
        setPixel( mx  + y, my + x, color );
        setPixel( mx  - y, my + x, color );
        setPixel( mx  - y, my - x, color );
        setPixel( mx  + y, my - x, color );

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * ( x - y ) + 10;
            y --;
        }
        x++;
    }
}

int LeftPixels(int x) 
{
	return 28 + 10 + x*60;
}

int TopPixels(int y) 
{
	return 6 + y*60;
}

void Mark(int x, int y, short color)
{
	
	int i,j;
	
    for(i = LeftPixels(x)+1; i < 28 + 10 + 59 + x*60; i++)
	for(j = TopPixels(y)+1; j < 65 + y*60; j++)
	{
		VRAM_A[256 * j + i] = color;
	}
}

void DrawX(int x, int y, short color)
{
	int lx=LeftPixels(x);
	int ly=TopPixels(y);
	
	Line(lx+23, ly+29, lx+6, ly+12, color);
	Line(lx+6, ly+12, lx+12, ly+6, color);
	Line(lx+12, ly+6, lx+29, ly+23, color);
	
	Line(lx+30, ly+23, lx+47, ly+6, color);
	Line(lx+47, ly+6, lx+53, ly+12, color);
	Line(lx+53, ly+12, lx+36, ly+29, color);
	
	Line(lx+36, ly+30, lx+53, ly+47, color);
	Line(lx+53, ly+47, lx+47, ly+53, color);
	Line(lx+47, ly+53, lx+30, ly+36, color);
	
	Line(lx+29, ly+36, lx+12, ly+53, color);
	Line(lx+12, ly+53, lx+6, ly+47, color);
	Line(lx+6, ly+47, lx+23, ly+30, color);
	
}

void DrawO(int x, int y, short color)
{
	int lx=LeftPixels(x);
	int ly=TopPixels(y);
	Circle( lx+29, ly+29, 24, color );
	Circle( lx+29, ly+29, 15, color );
}


void GetXY(touchPosition* touch, int* x, int* y) 
{
	*x = (touch->px - 38) / 60;
	*y = (touch->py - 6) / 60;
	
	if (*x > 2) { *x = 2; }
	if (*y > 2) { *y = 2; }
	if (*x < 0) { *x = 0; }
	if (*y < 0) { *y = 0; }
}

void SetUp() 
{

	videoSetMode(MODE_FB0);
	vramSetBankA(VRAM_A_LCD);
	
	videoSetModeSub(MODE_0_2D);	
	vramSetBankC(VRAM_C_SUB_BG);
	
	lcdMainOnBottom();
	
    ClearScreen();
}

void WaitForTouch(int *x, int* y) 
{
	touchPosition touch;
	while(1) 
	{
		swiWaitForVBlank();
		scanKeys();
		touchRead(&touch);
		int held = keysHeld();		// buttons currently held
		
		GetXY(&touch, x, y);
		
		if ( held & KEY_TOUCH ) 
		{
			break;
		}
	}
}

#define NOTHING 0
#define CROSS 1
#define NOUGHT 2

#define board(x,y) board[x+y*3]

int VictoryCheck(int* board)
{
	int i=0;
	// Check horizontal
	for (i=0;i<3;i++)
	{
		if (board(i,0) == board(i,1) &&  board(i,1) == board(i,2) && board(i,0)) 
		{
			Line(	LeftPixels(i) + 30, 
					TopPixels(0), 
					LeftPixels(i) + 30, 
					TopPixels(2) + 60, 
					RGB15(31,31,31)
				);
			return board(i,0); 
		}
	}
	
	// Check vertical
	for (i=0;i<3;i++)
	{
		if (board(0,i) == board(1,i) &&  board(1,i) == board(2,i) && board(0,i)) 
		{ 
			Line(	LeftPixels(0), 
					TopPixels(i) + 30, 
					LeftPixels(2) + 60, 
					TopPixels(i) + 30, 
					RGB15(31,31,31)
				);
			return board(0,i); 
		}
	}
	
	// Check diagonal
	if (board(0,0) == board(1,1) &&  board(1,1) == board(2,2) && board(0,0)) 
	{ 
			Line(	LeftPixels(0), 
					TopPixels(0) , 
					LeftPixels(2) + 60, 
					TopPixels(2) + 60, 
					RGB15(31,31,31)
				);
		return board(0,0); 
	}
	
	if (board(0,2) == board(1,1) &&  board(1,1) == board(2,0) && board(0,2)) 
	{ 
			Line(	LeftPixels(0), 
					TopPixels(2) + 60, 
					LeftPixels(2) + 60, 
					TopPixels(0), 
					RGB15(31,31,31)
				);
		return board(0,2); 
	}
	
	return NOTHING;
}

void ClearBoard(int* board) 
{
	int i;
	
	for(i=0;i<3;i++) 
	{
		board(i,0) = 0;
		board(i,1) = 0;
		board(i,2) = 0;
	}
}

int main(void) 
{
	SetUp();
  
	
	PrintConsole bottomScreen;
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&bottomScreen);

	int board[9];
	ClearBoard(board);
	
	int i=0;
	int x=0,y=0;
	int currentTurn = CROSS;
	int winner = NOTHING;
	int cnt = 0;
	
	int xscore=0;
	int oscore=0;
	
	consoleClear();
	iprintf("\n\n  X's Score:\n\n    %d\n\n  O's Score:\n\n    %d\n", xscore, oscore);
	if (currentTurn == CROSS)
	{
		iprintf("\n\n  X's Turn");
	}
	else
	{
		iprintf("\n\n  O's Turn");
	}
	
	while (1)
	{
		
		while (winner == NOTHING && cnt < 9) 
		{
			WaitForTouch(&x, &y);
			if (board(x,y) == NOTHING) 
			{
				board(x,y) = currentTurn;
				if (currentTurn == CROSS) 
				{
					DrawX(x,y,RGB15(0,31,0));
				}
				else
				{
					DrawO(x,y,RGB15(0,0,31));
				}
				winner = VictoryCheck(board);
				currentTurn = currentTurn ^ 3;
				cnt++;
				
				consoleClear();
				iprintf("\n\n  X's Score:\n\n    %d\n\n  O's Score:\n\n    %d\n", xscore, oscore);
				if (currentTurn == CROSS)
				{
					iprintf("\n\n  X's Turn");
				}
				else
				{
					iprintf("\n\n  O's Turn");
				}
			}
		}
		
		if (winner == CROSS) 
		{
			xscore++;
		}
		else if (winner == NOUGHT)
		{
			oscore++;
		}
			
		consoleClear();
		iprintf("\n\n  X's Score:\n\n    %d\n\n  O's Score:\n\n    %d\n", xscore, oscore);
		
	
		for (i=0;i<120;i++)
		{
			swiWaitForVBlank();
		}
		
				consoleClear();
				iprintf("\n\n  X's Score:\n\n    %d\n\n  O's Score:\n\n    %d\n", xscore, oscore);
				if (currentTurn == CROSS)
				{
					iprintf("\n\n  X's Turn");
				}
				else
				{
					iprintf("\n\n  O's Turn");
				}
				
		winner = NOTHING;
		ClearBoard(board);
		cnt = 0;
		ClearScreen();
	
	}
	
	return 0;
}
