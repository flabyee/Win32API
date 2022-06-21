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
    // �÷��̾� �̹���
    // ���⼭ ���� ��ȯ 3, 4, 5, 6
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

    // x��ǥ�� 700����
    TCHAR userMsg[256];
    wsprintf(userMsg, _T("Sokoban !"));
    TextOut(hdc, 700, 20, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("Q : ����"));
    TextOut(hdc, 700, 40, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("R : �ٽý���"));
    TextOut(hdc, 700, 60, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("N : ����"));
    TextOut(hdc, 700, 80, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("P : ����"));
    TextOut(hdc, 700, 100, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("Z : ��"));
    TextOut(hdc, 700, 120, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("X : ����"));
    TextOut(hdc, 700, 140, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("T : TestClear"));
    TextOut(hdc, 700, 160, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("�������� : %d"), nStage);
    TextOut(hdc, 700, 180, userMsg, lstrlen(userMsg));
    wsprintf(userMsg, _T("������ Ƚ�� : %d"), nMove);
    TextOut(hdc, 700, 200, userMsg, lstrlen(userMsg));
}

BOOL Game::TestEnd()
{
    for (int y = 0; y < 18; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (arStage[nStage][y][x] == '.' && ns[y][x] != 'O') {  //�÷��̾��� ������ġ
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

    TCHAR userMsg[256]; // ����ڿ��� ��� �޽����� �����ص� ����

    if (nStage < MAX_STAGE - 1)
    {
        wsprintf(userMsg,
            _T("%d ���������� Ǯ�����ϴ�.")_T("���� ���������� �̵��մϴ�"),
            GetCurrentStage() + 1);
    }
    else
    {
        wsprintf(userMsg, _T("��� ���������� Ǯ�����ϴ�."));
    }
    MessageBox(hWndMain, userMsg, _T("�˸�"), MB_OK);


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
    int dx = 0, dy = 0; //��ŸX, ��ŸY
    BOOL bWithPack = FALSE; // �Ѱ� �Բ� �������°�?

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
        if (ns[ny + dy][nx + dx] == 'O') //���ڸ� �о���
        {
            if (ns[ny + dy * 2][nx + dx * 2] == ' ' || ns[ny + dy * 2][nx + dx * 2] == '.')
            {
                //���� �и��� �ڽ��� ��ġ�� ���� ���������ٸ� �������������� �������� ��ȯ
                ErasePack(nx + dx, ny + dy);
                ns[ny + dy * 2][nx + dx * 2] = 'O'; //�ڽ��� �о�
                bWithPack = TRUE;
            }
            else { //���� ������ �̵��Ұ����ϴٸ� �̵���ü�� ����ϱ� ���� return
                return;
            }
        }

        nx += dx;
        ny += dy;
        

        // ����

        

        if (isUndo == false && isRedo == false)
        {
            // �տ��� ������
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
    memcpy(ns, arStage[nStage], sizeof(ns)); //�޸� ī��

    for (int y = 0; y < 18; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (ns[y][x] == '@') {  //�÷��̾��� ������ġ
                nx = x;
                ny = y;
                ns[y][x] = ' ';  //�ش���ġ�� �÷��̾ ������ ����ó��
            }
        }
    }

    // ����
    // �� ���� �����̳� ��ü�� ���ٸ�
    // �ʱ�ȭ ���־�� �Ѵ�.
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

        hMemDC = CreateCompatibleDC(hdc); //�޸� DC����
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

        hMemDC = CreateCompatibleDC(hdc); //�޸� DC����
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
        InitStage(); // �������� �ʱ�ȭ

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
        InitStage(); // �������� �ʱ�ȭ

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
