// GDIProject.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "GDIProject.h"

#include <vector>
#include "PointPair.h"
using namespace std;

#define MAX_LOADSTRING 100
 
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

vector<PointPair> ptList;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GDIPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GDIPROJECT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GDIPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
#define M       5

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HPEN hBluePen;
    static BOOL bDraw = FALSE;
    static POINT ptStart, ptEnd;

    static POINT ptStarts[5];
    static POINT ptEnds[5];
    static int index = 0;

    switch (message)
    {
    case WM_CREATE:
        {
            hBluePen = CreatePen(PS_DASH, 1, RGB(0, 0, 255));
        }
        break;
    case WM_SIZE:
        InvalidateRect(hWnd, NULL, TRUE);
        bDraw = TRUE;
        break;
    case WM_LBUTTONDOWN:
        {
        
            while (ptList.size() != index)
            {
                ptList.pop_back();
            }

            // 화면 초기화 이후 시작 지점 끝 지점 설정, 드래깅 시작
            ptStart.x = LOWORD(lParam);
            ptStart.y = HIWORD(lParam);
            ptEnd = ptStart;
        }
        break;
    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON)
        {
            // 드래깅 중이라면 화면 지우고 끝지점 다시 설정
            HDC hdc = GetDC(hWnd);
            // #1 이전 라인을 삭제하는 모드
            int oldMode = SetROP2(hdc, R2_NOTXORPEN);
            HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hBluePen));

            MoveToEx(hdc, ptStart.x, ptStart.y, NULL);
            LineTo(hdc, ptEnd.x, ptEnd.y);

            // #2 새로운 라인을 그리는 모드
            ptEnd.x = LOWORD(lParam);
            ptEnd.y = HIWORD(lParam);

            MoveToEx(hdc, ptStart.x, ptStart.y, NULL);
            LineTo(hdc, ptEnd.x, ptEnd.y);

            SelectObject(hdc, hOldPen);
            SetROP2(hdc, oldMode);
            ReleaseDC(hWnd, hdc);
        }
        break;
    case WM_LBUTTONUP:
        {
            // 드래깅 끝
            bDraw = TRUE;

            PointPair pt(ptStart, ptEnd);
            ptList.push_back(pt);
            index++;


            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
        
    case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case 90:
                    if (index > 0)
                    {
                        index--;
                    }
                 
                    bDraw = TRUE;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                case 82:
                    if (index < ptList.size())
                    {
                        index++;
                    }

                    bDraw = TRUE;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
            }
        }
        break;

    case WM_PAINT:
        {
            // GDI를 쓰러면 DC(devece context)를 얻어야 함
            // GetDC, ReleaseDC
            // WM_PAINT에서는 Begin, End Paint 사용
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            /*if (bDraw)
            {
                MoveToEx(hdc, ptStart.x, ptStart.y, NULL);
                LineTo(hdc, ptEnd.x, ptEnd.y);

                Rectangle(hdc, ptStart.x - (M / 2), ptStart.y - (M / 2), ptStart.x + (M / 2), ptStart.y + (M / 2));
                Rectangle(hdc, ptEnd.x - (M / 2), ptEnd.y - (M / 2), ptEnd.x + (M / 2), ptEnd.y + (M / 2));
                bDraw = FALSE;
            }*/

            int nowIndex = 0;
            if (bDraw)
            {
                for (auto it = ptList.begin(); it != ptList.end(); ++it)
                {
                    if (nowIndex < index)
                    {
                        MoveToEx(hdc, it->start.x, it->start.y, NULL);
                        LineTo(hdc, it->end.x, it->end.y);

                        Rectangle(hdc, it->start.x - (M / 2), it->start.y - (M / 2), it->start.x + (M / 2), it->start.y + (M / 2));
                        Rectangle(hdc, it->end.x - (M / 2), it->end.y - (M / 2), it->end.x + (M / 2), it->end.y + (M / 2));
                        bDraw = FALSE;
                    }

                    nowIndex++;
                }
            }
            

           /* SetROP2(hdc, R2_XORPEN);

            LineTo(hdc, ptOldEndMouse.x, ptOldEndMouse.y);

            SetROP2(hdc, R2_BLACK);

            MoveToEx(hdc, ptStartMouse.x, ptStartMouse.y, NULL);*/

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        DeleteObject(hBluePen);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
