#pragma once
class MoveInfo
{
public:
	char dx : 3;		// ��Ʈ �ʵ�, 3��Ʈ�� ���ڴ�
	char dy : 3;		// ��Ʈ �ʵ�, 3��Ʈ�� ���ڴ�
	char dir : 8;		
	char bWithPack : 2;	// ��Ʈ �ʵ�, 2��Ʈ�� ���ڴ�

	MoveInfo(int dx, int dy, int dir, bool bWithPack)
	{
		this->dx = dx;
		this->dy = dy;
		this->dir = dir;
		this->bWithPack = bWithPack;
		// ��Ʈ �ʵ�� ���ϴ� ��Ʈ�� �߶� �������� ������ �ϴ� ��Ʈ�� �߷�������
	}
};

