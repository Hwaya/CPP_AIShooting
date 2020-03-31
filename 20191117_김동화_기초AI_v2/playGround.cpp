#include "stdafx.h"
#include "playGround.h"


playGround::playGround()
{
}


playGround::~playGround()
{
	
}

//�ʱ�ȭ�� ����� �ض�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HRESULT playGround::init()
{
	gameNode::init();
	Tank ref;
	ref.bulletType = general;
	ref.speed = TANKSPD;
	ref.axis.x = PLAYERLOCAX;
	ref.axis.y = TANKLOCAY;
	ref.angle = 0;
	ref.hp = TANKHP;
	ref.controlType = player;
	ref.generalfirecnt = 0;
	ref.guidedfirecnt = 0;
	ref.antifirecnt = 0;
	P1 = new Tank(ref);
	
	
	ref.axis.x = AILOCAX;
	ref.angle = M_PI; 
	ref.speed = TANKSPD;
	ref.controlType = ai;
	AI = new Tank(ref);
	

	P1->target = AI;
	AI->target = P1;
	AI->generalfirecnt = 0;
	AI->guidedfirecnt = 0;
	AI->antifirecnt = 0;
	AI->lockdirtemper = 0;

	P1->DrawLine();
	AI->DrawLine();
	return S_OK;
}

//�޸� ������ ����� �ض�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void playGround::release()
{
	gameNode::release();

}

//������ ����� �ض�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void playGround::update()
{
	gameNode::update();
	
	P1->TankUpdate();
	AI->TankUpdate();
}

//����� �׷���!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void playGround::render(HDC hdc)
{
	//�ǵ�������
	HDC backDC = this->getBackBuffer()->getMemDC();
	PatBlt(backDC, 0, 0, WINSIZEX, WINSIZEY, WHITENESS);
	//==================================================

	RectangleMakeCenter(backDC, WINSIZEX / 2, WINSIZEY - 50, WINSIZEX, 100);

	P1->Draw(backDC);
	AI->Draw(backDC);


	//===================================================
	//�� ���ߴ�
	this->getBackBuffer()->render(hdc, 0, 0);
}
