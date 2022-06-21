#pragma once
#include "framework.h"
#include "resource.h"
#include "MoveInfo.h"
#include <vector>
using namespace std;

#define MAX_STAGE 3
#define BW 32
#define BH 32


class Game
{
public:
    char arStage[MAX_STAGE][18][21] = {
        {
        "####################",
        "####################",
        "####################",
        "#####   ############",
        "#####O  ############",
        "#####  O############",
        "###  O O ###########",
        "### # ## ###########",
        "#   # ## #####  ..##",
        "# O  O   @      ..##",
        "##### ### # ##  ..##",
        "#####     ##########",
        "####################",
        "####################",
        "####################",
        "####################",
        "####################",
        "####################"
        },
        {
        "####################",
        "####################",
        "####################",
        "####################",
        "####..  #     ######",
        "####..  # O  O  ####",
        "####..  #O####  ####",
        "####..    @ ##  ####",
        "####..  # #  O #####",
        "######### ##O O ####",
        "###### O  O O O ####",
        "######    #     ####",
        "####################",
        "####################",
        "####################",
        "####################",
        "####################",
        "####################"
        },
        {
        "####################",
        "####################",
        "####################",
        "####################",
        "##########     @####",
        "########## O#O #####",
        "########## O  O#####",
        "###########O O #####",
        "########## O # #####",
        "##....  ## O  O  ###",
        "###...    O  O   ###",
        "##....  ############",
        "####################",
        "####################",
        "####################",
        "####################",
        "####################",
        "####################"
        },
    };

    vector<MoveInfo>* undoList; // �̰� ���������� �ʴ´�.
    int nIndex; // undo redo �� ���ؼ�

    HWND hWndMain;
    Game(HWND main, HINSTANCE hInst);
    ~Game();

    void DrawScreen(HDC hdc, PAINTSTRUCT& ps);
    BOOL TestEnd(); // ���� Ŭ���� Ȯ��
    void TestClear();
    void Move(int dir, bool isUndo, bool isRedo);
    void InitStage();   // �������� �ʱ�ȭ
    void DrawBitMap(HDC hdc, int idx, int x, int y, HBITMAP hBitSheet);

    BOOL GotoNextStage();
    BOOL GotoPrevStage();
    void ErasePack(int x, int y);   // ���ڰ� �ִ� �ڸ��� ���� ���������ִ� �Լ�

    BOOL Undo();
    BOOL Redo();

    int GetCurrentStage();  // ���� �������� ���� �˾ƿ���

    BOOL isStageEnd = FALSE;
private:
    char ns[18][21] = { 0, };   // ���� �÷������� �� ����
    int nStage = 0;
    int nx = 0, ny = 0;
    int nMove = 0;
    HBITMAP hBitSheet = nullptr;  // ��Ʈ��
    HBITMAP hBitMap = nullptr;
};

