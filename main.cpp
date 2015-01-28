#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <future>
#include <algorithm>
#include "player.h"
#include "game.h"
using std::string;
using std::to_string;
#define clBackgroundColor RGB(255,255,255)
#define clLineColor RGB(232, 180, 255)
#define clBoxLineColor RGB(111, 0, 255)
#define clRedColor RGB(231, 69, 96)
#define clCellColor RGB(231, 69, 96)

/* system setting vars */
int cellSize = 20;
int xCell = 25;
int yCell = 28;
int width = xCell*cellSize;
int height = yCell*cellSize;
int speed = 1;
bool multithreading = true;

Player one, two;
Game game(one, two);
/* window procedure */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdParam, int cmdShow)
{
    MSG messages;        /* for messages queue manipulation */
    WNDCLASSEX WndClass; /* data struct for window class */
    HWND hWnd;           /* handle for window */

    /* define window class */
    WndClass.cbSize = sizeof(WNDCLASSEX);
    WndClass.style = CS_DBLCLKS;
    WndClass.hInstance = hInst;
    WndClass.lpszClassName = "WindowClassName";
    WndClass.lpfnWndProc = WndProc;

    /* icons, cursor and menu */
    WndClass.hIcon = LoadIcon(hInst, "MAINICON"); /* default icon */
    WndClass.hIconSm = LoadIcon(hInst, "MAINICON"); /* default icon */
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW); /* cursor */
    WndClass.lpszMenuName = NULL; /* no menu */
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;

    /* window background color */
    WndClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    RegisterClassEx(&WndClass);
    hWnd = CreateWindowEx(0,                     /* extended window style */
                          "WindowClassName",     /* registered class */
                          "Sea-Battle", /* window title */
                          (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),   /* default window style */
                          CW_USEDEFAULT,         /* x position */
                          CW_USEDEFAULT,         /* y position */
                          width,                   /* width of window */
                          height,                   /* heigth of window */
                          HWND_DESKTOP,          /* The window is a child-window to desktop */
                          NULL,                  /* no menu */
                          hInst,                 /* Program Instance handler */
                          NULL);                 /* No Window Creation data */


    HMENU hmenu1;
    hmenu1 = CreateMenu();

    AppendMenu(hmenu1, MF_STRING, 0, "&File");
    AppendMenu(hmenu1, MF_STRING, 1, "&Edit");
    AppendMenu(hmenu1, MF_STRING, 2, "&Help");
    ModifyMenu(hmenu1, 2, MF_GRAYED, 2, MF_STRING);

    SetMenu(hWnd, hmenu1);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    /* loop messages. run until GetMessage return 0*/
    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages); /* translate virtual keys into character messages*/
        DispatchMessage(&messages);  /* Send message to WndProc */
    }
    /* return value to system */
    return static_cast<int>(messages.wParam);
}

void drawLine(HDC);
void drawText(HDC, string, int, int);
void drawArea(HDC);
void drawCell(HDC, int, int, int);
void drawPoint(HDC, int, int, int);
void drawCross(HDC, int, int, int);
std::vector<std::pair<int, int> > mas;
UINT TimerId;
std::future<bool> fut; /* for asynchrony */
/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hDC, memDC;
    HBITMAP hScreen, oldBtm;
    int xPos = 0, yPos = 0;
    switch (message)
    {
    case WM_LBUTTONDOWN:
        xPos = LOWORD(lParam)/cellSize;
        yPos = HIWORD(lParam)/cellSize-1;
        mas.push_back(std::make_pair(xPos, yPos));
        break;

    case WM_KEYDOWN:
        switch(wParam) {
        case 27: /* Key Esc */
            PostQuitMessage(0);
            break;
        case 96: /* NumPad 0 */
            one = Player(); two = Player();
            Sleep(1000);
            one.genNew();
            TimerId = SetTimer(hwnd, 2, speed, NULL);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0); /* send a WM_QUIT to the message queue */
        break;
    case WM_CREATE:
        SetTimer(hwnd, 1, 100, NULL);
        TimerId = SetTimer(hwnd, 2, speed, NULL);
        one.genNew();
        Sleep(1000);
        two.genNew();
        break;
    case WM_TIMER:
        switch (wParam)
        {
            case 2:
                /* Test multithreading and asynchrony */
                if(multithreading){
                    fut = std::async([&](){
                            if(!game.playerMove()){
                                KillTimer(hwnd, TimerId);
                                return true;
                            }else return false;
                            });
                    fut.get();
                }else
                    if(!game.playerMove()) KillTimer(hwnd, TimerId);
                break;
            case 1:
                hDC = GetDC(hwnd);

                memDC = CreateCompatibleDC(hDC);
                hScreen = CreateCompatibleBitmap(hDC, width, height);
                oldBtm = static_cast<HBITMAP>(SelectObject(memDC, hScreen));
                PatBlt(memDC, 0, 0, width, height, WHITENESS);

                drawArea(memDC);

                for(int i=0; i<10; i++)
                    for(int j=0; j<10; j++) {
                        if(one.checkCell(i,j)) drawCell(memDC, i+1, j+1, 1);

                        if(one.getShootCell(i, j)==-1) drawPoint(memDC, i+1, j+1, 2);
                        else if(one.getShootCell(i, j)==1) drawCross(memDC, i+1, j+1, 2);
                        if(one.getWoundCell(i, j)==-1) drawPoint(memDC, i+1, j+1, 1);
                        else if(one.getWoundCell(i, j)==1) drawCross(memDC, i+1, j+1, 1);

                        if(two.checkCell(i,j)) drawCell(memDC, i+1, j+1, 4);

                        if(two.getShootCell(i, j)==-1) drawPoint(memDC, i+1, j+1, 3);
                        else if(two.getShootCell(i, j)==1) drawCross(memDC, i+1, j+1, 3);
                        if(two.getWoundCell(i, j)==-1) drawPoint(memDC, i+1, j+1, 4);
                        else if(two.getWoundCell(i, j)==1) drawCross(memDC, i+1, j+1, 4);

                    }
                drawText(memDC, to_string(one.counter), xCell/2*cellSize, 2);
                drawText(memDC, to_string(two.counter), xCell/2*cellSize, 13*cellSize);
                
                BitBlt(hDC, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
                SelectObject(memDC, oldBtm);
                DeleteObject(hScreen);
                DeleteDC(memDC);
                ReleaseDC(hwnd, hDC);
                break;
        }
        break;
    default:                /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
void drawText(HDC hDC, string str, int x, int y) {
    SetBkMode(hDC, TRANSPARENT);
    TextOut(hDC, x, y, str.c_str(), static_cast<int>(str.length()));
}
void drawCell(HDC hDC, int x, int y, int k) {
    HBRUSH hBrush = CreateSolidBrush(clBackgroundColor);
    HPEN hPen = CreatePen(PS_SOLID, 3, clBoxLineColor);
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hDC, hBrush));
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hDC, hPen));

    if(k==0) Rectangle(hDC, x*cellSize, y*cellSize+cellSize, x*cellSize+cellSize+1, y*cellSize+cellSize*2+1);
    if(k==1) Rectangle(hDC, x*cellSize, y*cellSize+cellSize, x*cellSize+cellSize+1, y*cellSize+cellSize*2+1);
    if(k==2) Rectangle(hDC, 13*cellSize+x*cellSize, y*cellSize+cellSize, x*cellSize+14*cellSize+1, y*cellSize+cellSize*2+1);
    if(k==3) Rectangle(hDC, x*cellSize, y*cellSize+14*cellSize, x*cellSize+cellSize+1, y*cellSize+cellSize*15+1);
    if(k==4) Rectangle(hDC, 13*cellSize+x*cellSize, y*cellSize+14*cellSize, x*cellSize+14*cellSize+1, y*cellSize+cellSize*15+1);

    SelectObject(hDC, hOldBrush);
    SelectObject(hDC, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
void drawPoint(HDC hDC, int x, int y, int k) {
    HPEN hPen = CreatePen(PS_SOLID, 3, clRedColor);
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hDC, hPen));

    if(k==1) Ellipse(hDC, 0.46*cellSize+x*cellSize, y*cellSize+cellSize*1.46, 0.54*cellSize+x*cellSize+1, y*cellSize+cellSize*1.54+1);
    if(k==2) Ellipse(hDC, 13.46*cellSize+x*cellSize, y*cellSize+cellSize*1.46, 13.54*cellSize+x*cellSize+1, y*cellSize+cellSize*1.54+1);
    if(k==3) Ellipse(hDC, 0.46*cellSize+x*cellSize, y*cellSize+cellSize*14.46, 0.54*cellSize+x*cellSize+1, y*cellSize+cellSize*14.54+1);
    if(k==4) Ellipse(hDC, 13.46*cellSize+x*cellSize, y*cellSize+cellSize*14.46, 13.54*cellSize+x*cellSize+1, y*cellSize+cellSize*14.54+1);

    SelectObject(hDC, hOldPen);
    DeleteObject(hPen);
}
void drawCross(HDC hDC, int x, int y, int k) {
    HPEN hPen = CreatePen(PS_SOLID, 2, clRedColor);
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hDC, hPen));

    if(k==1) {
        MoveToEx(hDC, x*cellSize+2, y*cellSize+cellSize+2, NULL);
        LineTo(hDC, x*cellSize+cellSize-2, y*cellSize+cellSize*2-2);
        MoveToEx(hDC, x*cellSize+cellSize-2, y*cellSize+cellSize+2, NULL);
        LineTo(hDC, x*cellSize+2, y*cellSize+cellSize*2-2);
    } else if(k==2) {
        MoveToEx(hDC, 13*cellSize+x*cellSize+2, y*cellSize+cellSize+2, NULL);
        LineTo(hDC, x*cellSize+14*cellSize-2,  y*cellSize+cellSize*2-2);
        MoveToEx(hDC, 14*cellSize+x*cellSize-2, y*cellSize+cellSize+2, NULL);
        LineTo(hDC, 13*cellSize+x*cellSize+2, y*cellSize+cellSize*2-2);
    }else if(k==3) {
        MoveToEx(hDC, x*cellSize+2, y*cellSize+14*cellSize+2, NULL);
        LineTo(hDC, x*cellSize+cellSize-2, y*cellSize+cellSize*15-2);
        MoveToEx(hDC, x*cellSize+cellSize-2, y*cellSize+14*cellSize+2, NULL);
        LineTo(hDC, x*cellSize+2, y*cellSize+cellSize*15-2);
    } else if(k==4) {
        MoveToEx(hDC, 13*cellSize+x*cellSize+2, y*cellSize+14*cellSize+2, NULL);
        LineTo(hDC, x*cellSize+14*cellSize-2,  y*cellSize+cellSize*15-2);
        MoveToEx(hDC, 14*cellSize+x*cellSize-2, y*cellSize+14*cellSize+2, NULL);
        LineTo(hDC, 13*cellSize+x*cellSize+2, y*cellSize+cellSize*15-2);
    }

    SelectObject(hDC, hOldPen);
    DeleteObject(hPen);
}
void drawArea(HDC hDC) {
    drawLine(hDC);

    HPEN hPen = CreatePen(PS_SOLID, 3, clBoxLineColor);
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hDC, hPen));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hDC, GetStockBrush(NULL_BRUSH)));

    /* First Box */
    Rectangle(hDC, cellSize, 2*cellSize, 11*cellSize+1, 12*cellSize+1);
    for(int i=1; i<=10; i++) {
        drawText(hDC, string(1,static_cast<char>(64+i)), cellSize*i+3, cellSize+2);
        drawText(hDC, to_string(i), 2, cellSize*(i+1)+2);
    }

    /* Second Box */
    Rectangle(hDC, 14*cellSize, 2*cellSize, 24*cellSize+1, 12*cellSize+1);
    for(int i=1; i<=10; i++) {
        drawText(hDC, string(1,static_cast<char>(64+i)), cellSize*13+cellSize*i+3, cellSize+2);
        drawText(hDC, to_string(i), cellSize*13+2, cellSize*(i+1)+2);
    }

    /* Third Box */
    Rectangle(hDC, cellSize, 15*cellSize, 11*cellSize+1, 25*cellSize+1);
    for(int i=1; i<=10; i++) {
        drawText(hDC, string(1,static_cast<char>(64+i)), cellSize*i+3, 14*cellSize+2);
        drawText(hDC, to_string(i), 2, cellSize*(i+14)+2);
    }

    /* Fourth Box */
    Rectangle(hDC, 14*cellSize, 15*cellSize, 24*cellSize+1, 25*cellSize+1);
    for(int i=1; i<=10; i++) {
        drawText(hDC, string(1,static_cast<char>(64+i)), cellSize*13+cellSize*i+3, 14*cellSize+2);
        drawText(hDC, to_string(i), cellSize*13+2, cellSize*(i+14)+2);
    }

    SelectObject(hDC, hOldBrush);
    SelectObject(hDC, hOldPen);
    DeleteObject(hPen);
}
void drawLine(HDC hDC) {
    HBRUSH hBrush = CreateSolidBrush(clBackgroundColor);
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hDC, hBrush));

    /* Draw all window */
    Rectangle(hDC, 0, 0, width, height);

    SelectObject(hDC, hOldBrush);
    DeleteObject(hBrush);

    HPEN hPen = CreatePen(PS_SOLID, 1, clLineColor);
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hDC, hPen));

    /* Horizontal lines */
    for(int i=0; i<xCell; i++)
    {
        MoveToEx(hDC, i*cellSize, cellSize, NULL);
        LineTo(hDC, i*cellSize, height);
    }
    /* Vertical lines */
    for(int i=0; i<yCell; i++)
    {
        MoveToEx(hDC, 0, i*cellSize, NULL);
        LineTo(hDC, width, i*cellSize);
    }

    SelectObject(hDC, hOldPen);
    DeleteObject(hPen);
}
