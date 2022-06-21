#pragma once
class MoveInfo
{
public:
	char dx : 3;		// 비트 필드, 3비트만 쓰겠다
	char dy : 3;		// 비트 필드, 3비트만 쓰겠다
	char dir : 8;		
	char bWithPack : 2;	// 비트 필드, 2비트만 쓰겠다

	MoveInfo(int dx, int dy, int dir, bool bWithPack)
	{
		this->dx = dx;
		this->dy = dy;
		this->dir = dir;
		this->bWithPack = bWithPack;
		// 비트 필드는 원하는 비트만 잘라서 가져가지 때문에 하단 비트만 잘려나간다
	}
};

