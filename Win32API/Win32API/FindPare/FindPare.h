#pragma once

#include "framework.h"
#include "resource.h"
#include <memory>

using namespace std;

enum class Status
{
	HIDDEN = 1,		// 카드가 뒤집혀있을때
	FLIP = 2,		// 카드가 열려있을때
	TEMPFLIP = 3,	// 카드가 임시로 열려있을 때
	ANIM = 4,		// 카드 애니메이션 중일 때
};

enum class GameStatus
{
	PAUSED = 0,
	RUN = 1,
	HINT = 2,
	VIEW = 3,
	ANIM = 4,
};

struct CardCell
{
	int _num;	// 이 카드가 몇번 카드인가?
	Status _status;	// 이 카드의 현재 상태는 무엇인가? (뒤집혔냐? 열려있냐? 임시로 열려있냐?)
};