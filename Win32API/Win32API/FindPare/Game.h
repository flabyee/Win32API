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
	shared_ptr<POINT> GetTempFlip();	// ���� ���� ī�带 ��ȯ
	int GetRemainPair();	// ���� ������ ������ ��ȯ
	void DrawScreen(HDC hdc);
	void DrawBitmap(HDC hdc, int x, int y, int idx);
	void DrawBitmap(HDC hdc, float x, float y, float height, int idx, BOOL isDown);
private:
	HINSTANCE _hInst;
	HWND _hWnd;
	CardCell _cardCells[4][4];
	int _count;				// �õ� Ƚ��
	HBITMAP _hBitSheet;		// ī���Ʈ
	GameStatus _gStatus;	// ���� ���� ����
	float _viewTimer;		// �ӽ÷� �����ִ� �ð� Ÿ�̸�
	float _hintTimer;		// ��Ʈ �����ִ� �ð� Ÿ�̸�
	float _time;			// ���� �����ϰ� �ɸ� �ð�
	float _animTimer;		// �ִϸ��̼� �ɸ��� �ð�
	float _animStartTime;	// �ִϸ��̼� ������ �ð�
};