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

    vector<MoveInfo>* undoList; // 이건 구현하지는 않는다.
    int nIndex; // undo redo 를 위해서

    HWND hWndMain;
    Game(HWND main, HINSTANCE hInst);
    ~Game();

    void DrawScreen(HDC hdc, PAINTSTRUCT& ps);
    BOOL TestEnd(); // 게임 클리어 확인
    void TestClear();
    void Move(int dir, bool isUndo, bool isRedo);
    void InitStage();   // 스테이지 초기화
    void DrawBitMap(HDC hdc, int idx, int x, int y, HBITMAP hBitSheet);

    BOOL GotoNextStage();
    BOOL GotoPrevStage();
    void ErasePack(int x, int y);   // 상자가 있던 자리에 맵을 복구시켜주는 함수

    BOOL Undo();
    BOOL Redo();

    int GetCurrentStage();  // 현제 스테이지 정보 알아오기

    BOOL isStageEnd = FALSE;
private:
    char ns[18][21] = { 0, };   // 현재 플레이중인 맵 정보
    int nStage = 0;
    int nx = 0, ny = 0;
    int nMove = 0;
    HBITMAP hBitSheet = nullptr;  // 비트맵
    HBITMAP hBitMap = nullptr;
};

