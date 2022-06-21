#pragma once
#include "Tetris.h"
#include "Player.h"
#include <queue>

#define BW 10
#define BH 20
#define TS 24 //블럭 사이즈는 24픽셀

class Game : public enable_shared_from_this<Game>
{
public:
	~Game()
	{
		DeleteObject(_blockSheet);
	}

    void Init(HINSTANCE hInst, HWND hWnd);
    void Update(ULONGLONG tick);
    void Render(HDC hdc);

    void PrintTile(HDC hdc, int x, int y, Type t); //지정된 타입을 지정된 위치에 출력
    void DrawBitmap(HDC hdc, int x, int y, int idx);
    void GetInput(WPARAM keyCode);
    void StartGame();
    void PauseGame();
    void MakeNewBrick();
    void MakeNewQueue();
    void UpdateBoard(); // 지정된 게임판 새로 그리기
    Type GetAround(Point p, int brick, int rot);
    void TestFull();

    BOOL CheckPlayerRender(int x, int y);

private:
    const int _blockCount = 7;
	HBITMAP _blockSheet;
    HINSTANCE _hInst;
    HWND _hWndMain;
    Type _board[BH + 2][BW + 2];
    unique_ptr<Player> _player;

    // nowBrick은 player가 가지고 있는다
    int _nextBrick = -1;
    queue<int> brickQueue;
    int _score = 0;
    int _brickCount = 0;
    GameStatus _status = GameStatus::GAMEOVER;
    int _blockTime = 0;     // 블럭이 아래로 움직이는 딜레이
    int _currentTime = 0;

    // [blockIndex][회전상태]
    Point Shape[7][4][4] = {
        { {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
        { {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },
        { {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
        { {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} },
        { {0,0,-1,0,1,0,-1,-1}, {0,0,0,-1,0,1,-1,1}, {0,0,-1,0,1,0,1,1}, {0,0,0,-1,0,1,1,-1} },
        { {0,0,1,0,-1,0,1,-1}, {0,0,0,1,0,-1,-1,-1}, {0,0,1,0,-1,0,-1,1}, {0,0,0,-1,0,1,1,1} },
        { {0,0,-1,0,1,0,0,1}, {0,0,0,-1,0,1,1,0}, {0,0,-1,0,1,0,0,-1}, {0,0,-1,0,0,-1,0,1} },  };
};

