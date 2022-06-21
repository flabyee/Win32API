#include "Game.h"

Game::Game(HWND main, HINSTANCE hInst)
{
	hWndMain = main;
	hBitSheet = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SHEET));
	hBitMap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP4));
	undoList = new vector<MoveInfo>();
}

Game::~Game()
{
	DeleteObject(hBitSheet);
	delete undoList;
}

void Game::DrawScreen(HDC hdc, PAINTSTRUCT& ps)
{
    // 플레이어 이미지
    // 여기서 방향 전환 3, 4, 5, 6
    int iBit = 3;

    //EMPTY, MAN, PACK, TARGET, WALL
    for (int y = 0; y < 18; y++)
    {
        for (int x = 0; x < 20; x++) {
            switch (ns[y][x])
            {
            case '#':
                iBit = 0;
                break;
            case '.':
                iBit = 2;
                break;
            case 'O':
                iBit = 1;
                break;
            case ' ':
                iBit = 7;
                break;
            }

            DrawBitMap(hdc, iBit, x * BW, y * BH, hBitSheet);
        }
    }

    if (undoList->size() > 0 && nIndex > 0)
    {
        switch ((*undoList)[nIndex - 1].dir)
        {
        case VK_UP:
            iBit = 3;
            break;
        case VK_DOWN:
            iBit = 4;
            break;
        case VK_LEFT:
            iBit = 5;
            break;
        case VK_RIGHT:
            iBit = 6;
            break;
        }
    }
    else
    {
        iBit = 3;
    }
    DrawBitMap(hdc, iBit, nx * BW, ny * BH, hBitSheet);

    // x좌표는 700부터
    TCHAR userMsg[256];
    wsprintf(userMsg, _T("Sokoban !"));
    TextOut(hdc, 700, 20, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("Q : 종료"));
    TextOut(hdc, 700, 40, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("R : 다시시작"));
    TextOut(hdc, 700, 60, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("N : 다음"));
    TextOut(hdc, 700, 80, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("P : 이전"));
    TextOut(hdc, 700, 100, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("Z : 언도"));
    TextOut(hdc, 700, 120, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("X : 리도"));
    TextOut(hdc, 700, 140, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("T : TestClear"));
    TextOut(hdc, 700, 160, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("스테이지 : %d"), nStage);
    TextOut(hdc, 700, 180, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("움직인 횟수 : %d"), nMove);
    TextOut(hdc, 700, 200, userMsg, lstrlen(userMsg));
}

BOOL Game::TestEnd()
{
    for (int y = 0; y < 18; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (arStage[nStage][y][x] == '.' && ns[y][x] != 'O') {  //플레이어의 시작위치
                return FALSE;
            }
        }
    }

    return TRUE;
}

void Game::TestClear()
{
    for (int y = 0; y < 18; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (ns[y][x] == 'O') 
            {
                ns[y][x] = ' ';
            }
        }
    }

    for (int y = 0; y < 18; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (arStage[nStage][y][x] == '.')
            {
                ns[y][x] = 'O';
            }
        }
    }

    InvalidateRect(hWndMain, NULL, FALSE);

    TCHAR userMsg[256]; // 사용자에게 띄울 메시지를 저장해둘 변수

    if (nStage < MAX_STAGE - 1)
    {
        wsprintf(userMsg,
            _T("%d 스테이지를 풀었습니다.")_T("다음 스테이지로 이동합니다"),
            GetCurrentStage() + 1);
    }
    else
    {
        wsprintf(userMsg, _T("모든 스테이지를 풀었습니다."));
    }
    MessageBox(hWndMain, userMsg, _T("알림"), MB_OK);


    if (GotoNextStage())
    {
        
    }
    else 
    {
        isStageEnd = TRUE;

        InvalidateRect(hWndMain, NULL, TRUE);
    }
}

void Game::Move(int dir, bool isUndo, bool isRedo)
{
    int dx = 0, dy = 0; //델타X, 델타Y
    BOOL bWithPack = FALSE; // 팩과 함께 움직였는가?

    switch (dir)
    {
    case VK_LEFT:
        dx = -1;
        break;
    case VK_RIGHT:
        dx = 1;
        break;
    case VK_UP:
        dy = -1;
        break;
    case VK_DOWN:
        dy = 1;
        break;
    default:
        break;
    }

    if (isUndo)
    {
        dx *= -1;
        dy *= -1;
    }

    if (ns[ny + dy][nx + dx] != '#')
    {
        if (ns[ny + dy][nx + dx] == 'O') //상자를 밀었다
        {
            if (ns[ny + dy * 2][nx + dx * 2] == ' ' || ns[ny + dy * 2][nx + dx * 2] == '.')
            {
                //지금 밀리는 박스의 위치가 원래 목적지였다면 스테이지정보도 목적지로 변환
                ErasePack(nx + dx, ny + dy);
                ns[ny + dy * 2][nx + dx * 2] = 'O'; //박스를 밀어
                bWithPack = TRUE;
            }
            else { //상자 뒤쪽이 이동불가능하다면 이동자체를 취소하기 위해 return
                return;
            }
        }

        nx += dx;
        ny += dy;
        

        // 수행

        

        if (isUndo == false && isRedo == false)
        {
            // 앞에것 날리기
            while (undoList->size() != nIndex)
            {
                undoList->pop_back();
            }

            MoveInfo mInfo(dx, dy, dir, bWithPack);
            undoList->push_back(mInfo);
            nIndex++;

            nMove++;
        }
        else if(isUndo == true)
        {
            nMove--;

            if ((*undoList)[nIndex].bWithPack == true)
            {
                int bx = 0, by = 0;
                switch ((*undoList)[nIndex].dir)
                {
                case VK_LEFT:
                    bx = -1;
                    break;
                case VK_RIGHT:
                    bx = 1;
                    break;
                case VK_UP:
                    by = -1;
                    break;
                case VK_DOWN:
                    by = 1;
                    break;
                default:
                    break;
                }

                ns[ny + by][nx + bx] = 'O';
                ns[ny + by * 2][nx + bx * 2] = ' ';
            }
        }
        else if (isRedo == true)
        {
            nMove++;
        }


        InvalidateRect(hWndMain, NULL, FALSE);
    }

}

void Game::InitStage()
{
    memcpy(ns, arStage[nStage], sizeof(ns)); //메모리 카피

    for (int y = 0; y < 18; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (ns[y][x] == '@') {  //플레이어의 시작위치
                nx = x;
                ny = y;
                ns[y][x] = ' ';  //해당위치에 플레이어가 있으니 공백처리
            }
        }
    }

    // 수행
    // 뭘 쓰든 컨테이너 객체를 썻다면
    // 초기화 해주어야 한다.
    undoList->clear();
    nIndex = 0;

    nMove = 0;
    InvalidateRect(hWndMain, NULL, TRUE);
}

void Game::DrawBitMap(HDC hdc, int idx, int x, int y, HBITMAP hBitSheet)
{
    if (isStageEnd == false)
    {
        HDC hMemDC;
        HBITMAP hOldBitMap;
        int bx, by;
        BITMAP bit;

        hMemDC = CreateCompatibleDC(hdc); //메모리 DC생성
        hOldBitMap = static_cast<HBITMAP>(SelectObject(hMemDC, hBitSheet));

        GetObject(hBitSheet, sizeof(BITMAP), &bit);

        bx = bit.bmWidth;
        by = bit.bmHeight;

        StretchBlt(hdc, x, y, 32, 32, hMemDC, idx * 32, 0, 32, 32, SRCCOPY);
        //BitBlt(hdc, x, y, bx, by, hMemDC, 0, 0, SRCCOPY);
        SelectObject(hMemDC, hOldBitMap);
        DeleteDC(hMemDC);
    }
    else
    {
        HDC hMemDC;
        HBITMAP hOldBitMap;
        int bx, by;
        BITMAP bit;

        hMemDC = CreateCompatibleDC(hdc); //메모리 DC생성
        hOldBitMap = static_cast<HBITMAP>(SelectObject(hMemDC, hBitMap));

        GetObject(hBitMap, sizeof(BITMAP), &bit);

        bx = bit.bmWidth;
        by = bit.bmHeight;

        BitBlt(hdc, 0, 0, bx, by, hMemDC, 0, 0, SRCCOPY);
        SelectObject(hMemDC, hOldBitMap);
        DeleteDC(hMemDC);
    }
}

 
BOOL Game::GotoNextStage()
{
    isStageEnd = FALSE;

    if (nStage < MAX_STAGE - 1)
    {
        nStage++;
        InitStage(); // 스테이지 초기화

        return TRUE;
    }

    return FALSE;
}

BOOL Game::GotoPrevStage()
{
    isStageEnd = FALSE;

    if (nStage > 0)
    {
        nStage--;
        InitStage(); // 스테이지 초기화

        return TRUE;
    }

    return FALSE;
}

void Game::ErasePack(int x, int y)
{
    ns[y][x] = arStage[nStage][y][x] == '.' ? '.' : ' ';
}

BOOL Game::Undo()
{
    if (nIndex > 0)
    {
        nIndex--;

        Move((*undoList)[nIndex].dir, TRUE, FALSE);

        return TRUE;
    }
    return FALSE;
}

BOOL Game::Redo()
{
    if (nIndex < undoList->size())
    {
        nIndex++;

        Move((*undoList)[nIndex - 1].dir, FALSE, TRUE);

        return TRUE;
    }
    return FALSE;
}

int Game::GetCurrentStage()
{
    return nStage;
}

// undo, redo
