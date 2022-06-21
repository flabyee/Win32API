#pragma once
#include "Tetris.h"
#include "Player.h"
#include <queue>

#define BW 10
#define BH 20
#define TS 24 //�� ������� 24�ȼ�

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

    void PrintTile(HDC hdc, int x, int y, Type t); //������ Ÿ���� ������ ��ġ�� ���
    void DrawBitmap(HDC hdc, int x, int y, int idx);
    void GetInput(WPARAM keyCode);
    void StartGame();
    void PauseGame();
    void MakeNewBrick();
    void MakeNewQueue();
    void UpdateBoard(); // ������ ������ ���� �׸���
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

    // nowBrick�� player�� ������ �ִ´�
    int _nextBrick = -1;
    queue<int> brickQueue;
    int _score = 0;
    int _brickCount = 0;
    GameStatus _status = GameStatus::GAMEOVER;
    int _blockTime = 0;     // ���� �Ʒ��� �����̴� ������
    int _currentTime = 0;

    // [blockIndex][ȸ������]
    Point Shape[7][4][4] = {
        { {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
        { {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },
        { {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
        { {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} },
        { {0,0,-1,0,1,0,-1,-1}, {0,0,0,-1,0,1,-1,1}, {0,0,-1,0,1,0,1,1}, {0,0,0,-1,0,1,1,-1} },
        { {0,0,1,0,-1,0,1,-1}, {0,0,0,1,0,-1,-1,-1}, {0,0,1,0,-1,0,-1,1}, {0,0,0,-1,0,1,1,1} },
        { {0,0,-1,0,1,0,0,1}, {0,0,0,-1,0,1,1,0}, {0,0,-1,0,1,0,0,-1}, {0,0,-1,0,0,-1,0,1} },  };
};

