#pragma once
#include "FindPare.h"

#define CS 64
#define random(n) (rand() % n)

class Game
{
public:
	Game(HINSTANCE hInst, HWND hWnd);
	~Game();
	void InitGame();
	void Update(ULONGLONG tick);
	void HandleInput(shared_ptr<POINT> mouseCoord);
	shared_ptr<POINT> GetTempFlip();	// 현재 열린 카드를 반환
	int GetRemainPair();	// 아직 못맞춘 갯수를 반환
	void DrawScreen(HDC hdc);
	void DrawBitmap(HDC hdc, int x, int y, int idx);
	void DrawBitmap(HDC hdc, float x, float y, float height, int idx, BOOL isDown);
private:
	HINSTANCE _hInst;
	HWND _hWnd;
	CardCell _cardCells[4][4];
	int _count;				// 시도 횟수
	HBITMAP _hBitSheet;		// 카드시트
	GameStatus _gStatus;	// 현재 게임 상태
	float _viewTimer;		// 임시로 보여주는 시간 타이머
	float _hintTimer;		// 힌트 보여주는 시간 타이머
	float _time;			// 게임 시작하고 걸린 시간
	float _animTimer;		// 애니메이션 걸리는 시간
	float _animStartTime;	// 애니메이션 시작한 시간
};