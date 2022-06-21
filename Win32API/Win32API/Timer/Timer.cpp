// Timer.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Timer.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define R       100
#define RADIAN(X) (X * M_PI / 180)  
// 180 : PI = X : ??, PI / 180 => deg2Rad



// degree to radian

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

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
    LoadStringW(hInstance, IDC_TIMER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TIMER));

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TIMER));
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

   SendMessage(hWnd, WM_TIMER, 1, 0);

   return TRUE;
}

void DrawNeedle(double theta, POINT& center, double radius, HDC& hdc, HPEN& hPen)
{
    POINT pt;
    pt.x = static_cast<long>(cos(RADIAN(theta)) * (radius) + center.x + 0.5);
    pt.y = static_cast<long>(sin(RADIAN(theta)) * (radius) + center.y + 0.5);

    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));

    MoveToEx(hdc, center.x, center.y, NULL);
    LineTo(hdc, pt.x, pt.y);

    SelectObject(hdc, hOldPen);
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SYSTEMTIME st;

    double theta;
    POINT pt;   // 1, 2, .... 12를 그릴 포인트

    static RECT rtClient;
    static TCHAR szTime[128];   // 시간 문자열 저장용
    static TCHAR msg[128];  // 이건 사용자 메시지 저장용
    static double sec, min, hour;
    static HPEN hPens[3]; // 시침, 분침, 초침? 을 그릴 팬 색상
    static POINT ptCircle;  // 시계를 그릴 원의 중심

    switch (message)
    {
    case WM_CREATE:
    {
        hPens[0] = CreatePen(PS_SOLID, 8, RGB(255, 0, 0));
        hPens[1] = CreatePen(PS_SOLID, 4, RGB(0, 255, 0));
        hPens[2] = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));

        SetTimer(hWnd, 1, 1000, NULL);  // 1초에 한번씩
    }
        break;
    case WM_SIZE:
    {
        GetClientRect(hWnd, &rtClient);
        ptCircle.x = rtClient.right / 2;
        ptCircle.y = rtClient.bottom / 2;
    }
        break;
    case WM_TIMER:
    {
        GetLocalTime(&st);  // 현재 시간을 참조해서 구조체에 정보를 실어준다.
        wsprintf(szTime, _T("%d-%d-%d : %d:%d:%d"), 
            st.wYear, st.wMonth, st.wDay, 
            st.wHour, st.wMinute, st.wSecond);

        sec = st.wSecond;
        min = st.wMinute + st.wSecond / 60.0;
        hour = st.wHour + min / 60.0;

        InvalidateRect(hWnd, NULL, TRUE);
    }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            DrawText(hdc, szTime, -1, &rtClient, DT_CENTER);

            Ellipse(hdc, ptCircle.x - R, ptCircle.y - R, ptCircle.x + R, ptCircle.y + R);

            for (int i = 1; i <= 12; i++)
            {
                theta = (static_cast<double>(i) - 3) * 30;  // 여기에 i에 맞는 각도가 나와야 함
                pt.x = static_cast<long>(cos(RADIAN(theta)) * R + ptCircle.x + 0.5);   // cos 함수를 이용해서 x위치구하기
                pt.y = static_cast<long>(sin(RADIAN(theta)) * R + ptCircle.y + 0.5);   // sin 함수를 이용해서 x위치구하기


                TCHAR str[10];
                wsprintf(str, _T("%d"), i);

                int r = 10;
                RECT rect;
                SetRect(&rect, pt.x - r, pt.y - r, pt.x + r, pt.y + r);

                DrawText(hdc, str, -1, &rect, DT_CENTER);

                // 여기서 str을 pt위치에 출력
            }

            // hour
            theta = hour * 30 - 90;


            double thetas[3] = { (hour * 30 - 90), (min * 6 - 90), (sec * 6 - 90) };
            for (int i = 0; i < 3; i++)
            {
                DrawNeedle(thetas[i], ptCircle, (R - static_cast<double>(2 - i) * 10), hdc, hPens[i]);
            }
            
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        DeleteObject(hPens[0]);
        DeleteObject(hPens[1]);
        DeleteObject(hPens[2]);

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
