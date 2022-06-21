#include "Game.h"
#include <Windows.h>
#include <iostream>
#include <sstream>

Game::Game(HINSTANCE hInst, HWND hWnd)
{
	RECT crt;
	SetRect(&crt, 0, 0, CS * 4 + 250, CS * 4);	// ���� ���� ũ�� ���ؼ� ������ũ�� ����

	AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, TRUE);
	SetWindowPos(hWnd, nullptr, 0, 0, crt.right - crt.left, crt.bottom - crt.top,
		SWP_NOMOVE | SWP_NOZORDER);

	_hInst = hInst;
	_hWnd = hWnd;
	_hBitSheet = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	srand(GetTickCount64());
	_viewTimer = 0;
	_hintTimer = 0;
	_gStatus = GameStatus::PAUSED;
}

Game::~Game()
{
	DeleteObject(_hBitSheet);
}

void Game::InitGame()
{
	_count = 0;
	memset(_cardCells, 0, sizeof(_cardCells));

	int randomArr[16];
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			randomArr[i * 2 + j] = i + 1;
		}
	}
	for (int i = 0; i < 16; i++)
	{
		int randomIndex = random(16);
		int temp = randomArr[i];
		randomArr[i] = randomArr[randomIndex];
		randomArr[randomIndex] = temp;
	}

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			_cardCells[y][x]._num = randomArr[y * 4 + x];
			_cardCells[y][x]._status = Status::HIDDEN;
		}
	}

	_gStatus = GameStatus::HINT;
	InvalidateRect(_hWnd, nullptr, TRUE);
	_hintTimer = 2000;	// 2�ʵ��� ����(���ӽ����Ҷ�)
}

/*
	_count = 0;
	memset(_cardCells, 0, sizeof(_cardCells));
	int x = 0;
	int y = 0;

	for (int i = 0; i <= 8; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			do {
				x = random(4);
				y = random(4);
			} while (_cardCells[y][x]._num != 0);

			_cardCells[y][x]._num = i;
			_cardCells[y][x]._status = Status::HIDDEN;
		}
	}

	_gStatus = GameStatus::HINT;
	InvalidateRect(_hWnd, nullptr, TRUE);
	_hintTimer = 2000;	// 2�ʵ��� ����(���ӽ����Ҷ�)
*/

void Game::Update(ULONGLONG tick)
{
	if (_gStatus == GameStatus::PAUSED)
		return;

	_time += tick;

	RECT crt = { 64 * 4 + 110, 90, 64 * 4 + 250, 64 * 4 };
	//SetRect(&crt, CS * 4, CS * 4, CS * 4 + 250, CS * 4);	// ���� ���� ũ�� ���ؼ� ������ũ�� ����

	InvalidateRect(_hWnd, &crt, FALSE);


	if (_gStatus == GameStatus::VIEW)
	{
		_viewTimer -= tick;
		if (_viewTimer <= 0)
		{
			_viewTimer = 0;

			_gStatus = GameStatus::ANIM;
			_animTimer = 500;
			_animStartTime = _time;

			for (int x = 0; x < 4; x++)
			{
				for (int y = 0; y < 4; y++)
				{
					if (_cardCells[y][x]._status == Status::TEMPFLIP)
						_cardCells[y][x]._status = Status::ANIM;
				}
			}
			InvalidateRect(_hWnd, nullptr, FALSE);
		}
	}
	else if (_gStatus == GameStatus::ANIM)
	{
		_animTimer -= tick;

		if (_animTimer <= 0)
		{
			_animTimer = 0;

			_gStatus = GameStatus::RUN;

			for (int x = 0; x < 4; x++)
			{
				for (int y = 0; y < 4; y++)
				{
					if (_cardCells[y][x]._status == Status::ANIM)
						_cardCells[y][x]._status = Status::HIDDEN;
				}
			}
		}

		InvalidateRect(_hWnd, nullptr, FALSE);
	}
	else if (_gStatus == GameStatus::HINT)
	{
		_hintTimer -= tick;
		if (_hintTimer <= 0)
		{
			_gStatus = GameStatus::RUN;
			InvalidateRect(_hWnd, nullptr, FALSE);
		}
	}
}

void Game::HandleInput(shared_ptr<POINT> mouseCoord)
{
	int nx = mouseCoord->x;
	int ny = mouseCoord->y;

	if (_gStatus != GameStatus::RUN || nx > 3 || ny > 3
		|| _cardCells[ny][nx]._status != Status::HIDDEN)
	{
		return;
	}

	shared_ptr<POINT> p = GetTempFlip();	// ���� �ӽ÷� �����ִ� ī�尡 �ִ���
	
	// ù��°�� ����
	if (p == nullptr)
	{
		_cardCells[ny][nx]._status = Status::TEMPFLIP;
		InvalidateRect(_hWnd, nullptr, FALSE);
	}
	// �ι�°�� ����
	else
	{
		_count++;	// �õ�Ƚ�� ����
		if (_cardCells[p->y][p->x]._num == _cardCells[ny][nx]._num)	// ���� ī����
		{
			MessageBeep(0);	// �� �Ҹ�
			_cardCells[p->y][p->x]._status = Status::FLIP;
			_cardCells[ny][nx]._status = Status::FLIP;
			InvalidateRect(_hWnd, nullptr, FALSE);

			// Ŭ���� üũ
			if (GetRemainPair() == 0)
			{
				TCHAR msg[128];
				wsprintf(msg, _T("���� Ŭ������ %d �� �ɸ�"), static_cast<int>(_time / 1000));
				MessageBox(_hWnd, msg, _T("�˸�"), MB_OK);
				_gStatus = GameStatus::PAUSED;
			}
		}
		else
		{
			_cardCells[ny][nx]._status = Status::TEMPFLIP;
			InvalidateRect(_hWnd, nullptr, FALSE);
			_gStatus = GameStatus::VIEW;
			_viewTimer = 1000;
		}
	}

}




shared_ptr<POINT> Game::GetTempFlip()
{
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (_cardCells[y][x]._status == Status::TEMPFLIP)
			{
				shared_ptr<POINT> p = make_shared<POINT>();
				p->x = x;
				p->y = y;
				return p;
			}
		}
	}
	return nullptr;
}

int Game::GetRemainPair()
{
	int remain = 16;

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (_cardCells[y][x]._status == Status::FLIP)
			{
				remain--;
			}
		}
	}

	return remain / 2;
}

void Game::DrawScreen(HDC hdc)
{
	int imgIdx;

	if (_gStatus == GameStatus::HINT)
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				imgIdx = _cardCells[y][x]._num - 1;

				DrawBitmap(hdc, x * CS, y * CS, imgIdx);
			}
		}
	}
	else
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				if (_cardCells[y][x]._status == Status::FLIP
					|| _cardCells[y][x]._status == Status::TEMPFLIP)
				{
					imgIdx = _cardCells[y][x]._num - 1;

					DrawBitmap(hdc, x * CS, y * CS, imgIdx);
				}
				else if (_cardCells[y][x]._status == Status::HIDDEN)
				{
					imgIdx = 8;	// ����ǥ �̹���

					DrawBitmap(hdc, x * CS, y * CS, imgIdx);
				}
				else if (_cardCells[y][x]._status == Status::ANIM)
				{
					imgIdx = _cardCells[y][x]._num - 1;

					float progress = ((_time - _animStartTime) / 500) * CS;	// �ִϸ��̼� �����

					std::wostringstream os_;    
					os_ << progress << "\n";
					OutputDebugStringW(os_.str().c_str());  

					DrawBitmap(hdc, x * CS, y * CS, progress, 8, TRUE);

					DrawBitmap(hdc, x * CS, y * CS + progress, CS - progress, imgIdx, FALSE);
				}
			}
		}
	}


	TCHAR msg[128];
	lstrcpy(msg, _T("¦ ���߱� ���� Ver 0.01"));
	TextOut(hdc, 300, 10, msg, lstrlen(msg));

	wsprintf(msg, _T("�� �õ� Ƚ�� : %d"), _count);
	TextOut(hdc, 300, 50, msg, lstrlen(msg));

	wsprintf(msg, _T("���� �� ã�� �� %d"), GetRemainPair());
	TextOut(hdc, 300, 70, msg, lstrlen(msg));

	wsprintf(msg, _T("�帥 �ð� %d"), static_cast<int>(_time / 1000));
	TextOut(hdc, 300, 90, msg, lstrlen(msg));
}

void Game::DrawBitmap(HDC hdc, int x, int y, int idx)
{
	HDC hMemDC;
	HBITMAP hOldBitmap;

	hMemDC = CreateCompatibleDC(hdc);	// �־��� hdc�� ȣȯ�Ǵ� MemDC ����
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, _hBitSheet);

	StretchBlt(hdc, x, y, CS, CS, hMemDC, idx * CS, 0, CS, CS, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Game::DrawBitmap(HDC hdc, float x, float y, float height, int idx, BOOL isDown)
{
	HDC hMemDC;
	HBITMAP hOldBitmap;

	hMemDC = CreateCompatibleDC(hdc);	// �־��� hdc�� ȣȯ�Ǵ� MemDC ����
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, _hBitSheet);

	StretchBlt(hdc, 
		x, y, 
		CS, height, 
		hMemDC, 
		idx * CS, isDown ? CS - height : 0, 
		CS, height, 
		SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}