#include "Game.h"

void Game::Init(HINSTANCE hInst, HWND hWnd)
{
    _player = make_unique<Player>(BW / 2, 3, shared_from_this());    // 이 부분은 나중에 변경

    _status = GameStatus::GAMEOVER; //처음 시작시에 게임오버 상태로 놓고
    _hInst = hInst;
    _hWndMain = hWnd;

    RECT clientRect;
    SetRect(&clientRect, 0, 0, (BW + 12) * TS, (BH + 2) * TS);
    AdjustWindowRect(&clientRect, WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX, TRUE);
    SetWindowPos(hWnd, NULL, 0, 0,
        clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
        SWP_NOMOVE | SWP_NOZORDER);

    srand(static_cast<unsigned int>(GetTickCount64()));
    _blockSheet = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BLOCKSHEET)); //블럭시트 로딩
}



void Game::Update(ULONGLONG tick)
{
    if (_status == GameStatus::RUNNING)
        _currentTime += static_cast<int>(tick);

    if (_currentTime > _blockTime)
    {
        _currentTime = 0;
        if (_player->MoveDown() == TRUE)
        {
            MakeNewBrick();
        }
    }
}

void Game::Render(HDC hdc)
{
    //화면 외곽을 그리는거고
    for (int x = 0; x < BW + 1; x++)
    {
        PrintTile(hdc, x, 0, Type::WALL);
        PrintTile(hdc, x, BH + 1, Type::WALL);
    }

    for (int y = 0; y < BH + 2; y++)
    {
        PrintTile(hdc, 0, y, Type::WALL);
        PrintTile(hdc, BW + 1, y, Type::WALL);
    }

    //보드를 그리는 거고
    for (int y = 1; y < BH + 1; y++)
    {
        for (int x = 1; x < BW + 1; x++)
        {
            if (CheckPlayerRender(x, y))
            {
                PrintTile(hdc, x, y, _player->GetBrickType());
            }
            else
            {
                PrintTile(hdc, x, y, _board[y][x]);
            }
        }
    }


    //다음 모양 그리기
   //외곽 테두리
    for (int x = BW + 3; x <= BW + 11; x++)
    {
        for (int y = BH - 5; y <= BH + 1; y++)
        {
            if (x == BW + 3 || x == BW + 11 || y == BH - 5 || y == BH + 1) {
                PrintTile(hdc, x, y, Type::WALL);
            }
            else {
                PrintTile(hdc, x, y, Type::EMPTY);
            }
        }
    }

    //다음블럭 그리기
    if (_status != GameStatus::GAMEOVER)
    {
        for (int i = 0; i < 4; i++)
        {
            PrintTile(hdc,
                BW + 7 + Shape[_nextBrick][0][i].x,
                BH - 2 + Shape[_nextBrick][0][i].y,
                static_cast<Type>((_nextBrick + 1)));
        }
    }


    // 고스트
    if (_status != GameStatus::GAMEOVER)
    {
        Point p = _player->GetPoint();
        int _ghostBrick = _player->GetBrickNum();
        int _rot = _player->GetRot();
        BOOL b = FALSE;
        while (b == FALSE)
        {
            p.y += 1;
            if (GetAround(p, _ghostBrick, _rot) != Type::EMPTY)
            {
                b = TRUE;
            }
        }

        p.y--;

        for (int i = 0; i < 4; i++)
        {
            PrintTile(hdc,
                p.x + Shape[_ghostBrick][_rot][i].x,
                p.y + Shape[_ghostBrick][_rot][i].y,
                static_cast<Type>((_ghostBrick + 1)));
        }
    }


    TCHAR msgStr[256];
    lstrcpy(msgStr, _T("GGM Tetris1"));
    TextOut(hdc, (BW + 4) * TS, 30, msgStr, lstrlen(msgStr));
    wsprintf(msgStr, _T("점수 : %d "), _score);
    TextOut(hdc, (BW + 4) * TS, 60, msgStr, lstrlen(msgStr));
    wsprintf(msgStr, _T("벽돌 : %d 개"), _brickCount);
    TextOut(hdc, (BW + 4) * TS, 90, msgStr, lstrlen(msgStr));
}

void Game::PrintTile(HDC hdc, int x, int y, Type t)
{
    DrawBitmap(hdc, x * TS, y * TS, static_cast<int>(t));
}

void Game::DrawBitmap(HDC hdc, int x, int y, int idx)
{
    int sx = idx % 4;
    int sy = idx / 4;

    HDC hMemDC;
    HBITMAP oldBitmap;
    hMemDC = CreateCompatibleDC(hdc);
    oldBitmap = static_cast<HBITMAP>(SelectObject(hMemDC, _blockSheet));

    StretchBlt(hdc, x, y, TS, TS, hMemDC, sx * TS, sy * TS, TS, TS, SRCCOPY);
    SelectObject(hMemDC, oldBitmap);
    DeleteDC(hMemDC);
}

void Game::GetInput(WPARAM keyCode)
{
    if (_status == GameStatus::RUNNING)
        _player->HandleInput(keyCode);
}

void Game::StartGame()
{
    if (_status != GameStatus::GAMEOVER) return;

    for (int y = 0; y < BH + 2; y++)
    {
        for (int x = 0; x < BW + 2; x++) {
            _board[y][x] = (x == 0 || x == BW + 1 || y == 0 || y == BH + 1)
                ? Type::WALL : Type::EMPTY;
        }
    }

    _score = 0;
    _brickCount = 0;
    _status = GameStatus::RUNNING;

    if (brickQueue.empty())
    {
        MakeNewQueue();
    }
    _nextBrick = brickQueue.front();
    brickQueue.pop();

    MakeNewBrick(); // nextBrick => nowBrick 으로 변경하고 nextBrick하나 새로 뽑고
    _blockTime = 1000; //1초에 한번씩만 내려오도록 해준다.

    //OutputDebugString(_T("Hello"));
}

void Game::PauseGame()
{
    if (_status == GameStatus::RUNNING) {
        _status = GameStatus::PAUSE;
    }
    else if (_status == GameStatus::PAUSE) {
        _status = GameStatus::RUNNING;
    }
}

void Game::MakeNewBrick()
{
    _brickCount++;
    _player->SetBrickNum(_nextBrick);
    _player->SetPos(BW / 2, 3);
    _player->SetRot(0);


    if (brickQueue.empty())
    {
        MakeNewQueue();
    }
    _nextBrick = brickQueue.front();
    brickQueue.pop();

    InvalidateRect(_hWndMain, NULL, FALSE); //이건 나중에 개선할거다.
    Type t = GetAround(_player->GetPoint(), _player->GetBrickNum(), _player->GetRot());
    if (t != Type::EMPTY)
    {
        _status = GameStatus::GAMEOVER;
        MessageBox(_hWndMain, _T("Game over!, 다시 하려면 게임시작을 누르세요"), _T("알림"), MB_OK);
    }
}

void Game::MakeNewQueue()
{
    int randomArr[7] = { 0,1,2,3,4,5,6 };
    for (int i = 0; i < 7; i++)
    {
        int randomIndex = rand() % (7 - i);
        brickQueue.push(randomArr[randomIndex]);
        randomArr[randomIndex] = randomArr[7 - i - 1];
    }

    for (int i = 0; i < 7; i++)
    {
        
    }
}

void Game::UpdateBoard()
{
    RECT rt;
    // 게임화면 다시 그리기
    SetRect(&rt, TS, TS, (BW + 1) * TS, (BH + 1) * TS);
    InvalidateRect(_hWndMain, &rt, FALSE);

    // 다음블럭 모양 다시그리기
    SetRect(&rt, (BW + 4) * TS, (BH - 6) * TS, (BW + 10) * TS, (BH) * TS);
    InvalidateRect(_hWndMain, &rt, FALSE);
}

Type Game::GetAround(Point p, int brick, int rot)
{
    for (int i = 0; i < 4; i++)
    {
        Type t = _board[p.y + Shape[brick][rot][i].y][p.x + Shape[brick][rot][i].x];
        if (t != Type::EMPTY)
            return t;
    }
    return Type::EMPTY;
}

void Game::TestFull()
{
    int count = 0;
    Point p = _player->GetPoint();
    int nowBrick = _player->GetBrickNum();
    int rot = _player->GetRot();
    Type t = _player->GetBrickType();

    // 내가 가지고 있던 블럭을 board에 기록
    for (int i = 0; i < 4; i++)
    {
        _board[p.y + Shape[nowBrick][rot][i].y][p.x + Shape[nowBrick][rot][i].x] = t;
    }
    nowBrick = -1;


    // 그 후에 보드를 검사해서 꽉찬 줄은 삭제
    for (int y = 1; y < BH + 1; y++)
    {
        BOOL isClear = TRUE;

        for (int x = 1; x < BW + 1; x++)
        {
            if (_board[y][x] == Type::EMPTY)
            {
                isClear = FALSE;
                break;
            }
        }

        if (isClear)
        {
            // 해당 줄 지우기(어차피 당겨지면서 저절로 지워진다)할필요없음
            /*for (int x = 1; x < BW + 1; x++)
            {
                board[y][x] = Type::EMPTY;
            }*/

            // 당기기
            for (int ty = y; ty > 1; ty--)
            {
                for (int x = 1; x < BW + 1; x++)
                {
                    _board[ty][x] = _board[ty - 1][x];    // 윗줄 데이터를 현재줄로 복사
                }
            }

            for (int x = 1; x < BW + 1; x++)
            {
                _board[1][x] = Type::EMPTY;
            }
            //InvalidateRect(_hWndMain, NULL, FALSE);
            UpdateBoard();

            count++;
        }
    }

    switch (count)
    {
    case 1:
        _score += 1;
        break;
    case 2:
        _score += 4;
        break;
    case 3:
        _score += 8;
        break;
    case 4:
        _score += 16;
        break;
    }

    if (_brickCount % 10 == 0 && _blockTime > 200)
    {
        _blockTime -= 50;
        //SetTimer(hWndMain, 1, blockInterval, NULL);
    }
}

BOOL Game::CheckPlayerRender(int x, int y)
{
    if (_status != GameStatus::GAMEOVER && _player->GetBrickNum() != -1)
    {
        for (int idx = 0; idx < 4; idx++)
        {
            Point p = _player->GetPoint();
            int rot = _player->GetRot();
            int nowBrick = _player->GetBrickNum();

            if (x == p.x + Shape[nowBrick][rot][idx].x
                && y == p.y + Shape[nowBrick][rot][idx].y)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}
