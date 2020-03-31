#pragma once
#include "gameNode.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#define TANKHP 100
#define TANKSPD 5
#define TANKSIZE 30
#define TANKHP 300

#define PLAYERLOCAX 150
#define AILOCAX WINSIZEX - 150
#define TANKLOCAY WINSIZEY - 100 -TANKSIZE / 2

#define GRAVITY 0.08

#define GENERALDMG 20
#define GENERALSPEED 5
#define GENERALSIZE 8

#define GUIDEDDMG 10
#define GUIDEDSPEED 5
#define GUIDEDSIZE 15
#define GUIDEDMINH WINSIZEY/2

#define ANTIDMG 0
#define ANTISPEED 10
#define ANTISIZE 10

#define HPBARSIZE TANKSIZE*2

#define LINESIZE 7

#define AIMSPD 0.01
#define GENERALFIRERATE 15
#define GUIDEDFIRERATE 30
#define ANTIFIRERATE 50
 
#define EVADESPD 3

enum Bullet_Type {
	general,
	guided,
	anti_guided
};

enum Control_Type {
	player,
	ai
};

enum State_Type {
	idle,
	move,
	attack,
	evade
};

enum Move_direction {
	left,
	right
};

template <typename T>
struct Bullet {
	POINTFLOAT axis;
	float angle;
	float speed;
	float gravity;
	float damage;
	float size;
	T* target;
	Bullet* targetBullet;
	Bullet_Type bulltype;
	RECT collision;
	bool locked;
	bool amIDestroyed;

	virtual void BulletMove() {
		if (!amIDestroyed) {
			switch (bulltype) {
			case general:
				axis.x += cosf(angle) * speed;
				axis.y += sinf(angle) * speed;
				gravity += GRAVITY;
				axis.y += gravity;
				break;
			case guided:
				if (axis.y > GUIDEDMINH && locked == false) {
					axis.y -= speed;
				}
				else if (locked == false) {
					locked = true;
					float dx = target->axis.x;
					float dy = target->axis.y;

					angle = atan2f(dy - axis.y, dx - axis.x);
				}
				else {
					axis.x += cosf(angle) * speed;
					axis.y += sinf(angle) * speed;
				}
				break;
			case anti_guided:
				if (target->bvec.size() > 0) {
					for (size_t t = 0; t < target->bvec.size(); t++) {
						if (target->bvec[t].bulltype == guided) {
							targetBullet = &target->bvec[t];
							float dx = targetBullet->axis.x;
							float dy = targetBullet->axis.y;
							angle = atan2f(dy - axis.y, dx - axis.x);
							locked = true;
							break;
						}
						else {
							angle = -M_PI / 2;
							locked = false;
						}
					}
				}
				axis.x += cosf(angle) * speed;
				axis.y += sinf(angle) * speed;
				break;
			}
			OutOfRange();
			MakeCollision();
		}
	}
	void OutOfRange() {
		if (axis.x < 0 || axis.x > WINSIZEX || axis.y <0 || axis.y > WINSIZEY - 100) {
			amIDestroyed = true;
		}
	}
	virtual void MakeCollision() {
		collision = RectMake(axis.x, axis.y, size, size);
	}
};

struct Tank {
	typedef vector<Bullet<Tank>>::iterator blitr;
	vector<Bullet<Tank>> bvec;
	
	Bullet_Type bulletType;
	State_Type stateType;
	Control_Type controlType;
	POINTFLOAT axis;
	float angle;
	float speed;
	int hp;
	RECT hpBar;
	RECT hpBarUnder;
	RECT gaugeBar;
	RECT gaugeBarUnder;
	RECT NameTag;
	int _r, _g, _b;
	RECT collision;
	Tank* target;
	vector<Bullet<Tank>> linevec; 
	bool lockon;
	POINTFLOAT lineLastAxis;
	int _lr, _lg, _lb;
	int generalfirecnt;
	int guidedfirecnt;
	int antifirecnt;
	State_Type curstate;

	bool lockdirection;
	int lockdirtemper;

	/// Controller
	void Controller() {
		if (controlType == player) {
			if (KEYMANAGER->isStayKeyDown(VK_LEFT)) {
				MoveController(-1);
				DrawLine();
			}
			else if (KEYMANAGER->isStayKeyDown(VK_RIGHT)) {
				MoveController(1);
				DrawLine();
			}
			if (KEYMANAGER->isStayKeyDown(VK_UP)) {
				angle -= AIMSPD;
				DrawLine();
			}
			else if (KEYMANAGER->isStayKeyDown(VK_DOWN)) {
				angle += AIMSPD;
				DrawLine();
			}
			if (KEYMANAGER->isOnceKeyDown(VK_SPACE)) {
				if (generalfirecnt > GENERALFIRERATE) {
					Fire(general);
				}
			}
			if (KEYMANAGER->isOnceKeyDown(0x5A)) {
				if (guidedfirecnt > GUIDEDFIRERATE) {
					Fire(guided);
				}
			}
			if (KEYMANAGER->isOnceKeyDown(0x58)) {
				if (antifirecnt > ANTIFIRERATE) {
					Fire(anti_guided);
				}
			}
		}
		else {
			AI_Control();
		}
	}
	void MoveController(int x) {
		if(axis.x >= TANKSIZE && axis.x <= WINSIZEX - TANKSIZE){
			if (curstate != evade) {
				Move(x, speed);
			}
			else{
				Move(x, EVADESPD);
			}
			DrawLine();
		}
		else if (axis.x <= TANKSIZE)axis.x = TANKSIZE;
		else if (axis.x >= WINSIZEX - TANKSIZE)axis.x = WINSIZEX - TANKSIZE ;
	}
	void Move(int x, float spd) {
		axis.x += x * spd;
	}
	void Fire(Bullet_Type bull) {
		Bullet<Tank>* newBullet = new Bullet<Tank>();

		switch (bull) {
		case general:
			newBullet->angle = angle;
			newBullet->axis.x = axis.x + cosf(angle) * 50;
			newBullet->axis.y = axis.y + sinf(angle) * 50;
			newBullet->damage = GENERALDMG;
			newBullet->speed = GENERALSPEED;
			newBullet->size = GENERALSIZE;
			newBullet->bulltype = general;
			generalfirecnt = 0;
			break;
		case guided:
			newBullet->angle = 0;
			newBullet->axis.x = axis.x;
			newBullet->axis.y = axis.y - TANKSIZE / 2;
			newBullet->damage = GUIDEDDMG;
			newBullet->speed = GUIDEDSPEED;
			newBullet->size = GUIDEDSIZE;
			newBullet->bulltype = guided;
			newBullet->target = target;
			guidedfirecnt = 0;
			break;
		case anti_guided:
			newBullet->angle = M_PI / 2;
			newBullet->axis.x = axis.x;
			newBullet->axis.y = axis.y - TANKSIZE / 2;
			newBullet->damage = ANTIDMG;
			newBullet->speed = ANTISPEED;
			newBullet->size = ANTISIZE;
			newBullet->bulltype = anti_guided;
			newBullet->target = target;
			antifirecnt = 0;
			break;
		}
		bvec.push_back(*newBullet);
	}
	/// ---Controller

	/// AI Controller
	void AI_Control() {
		// Fire
		if (lockon & generalfirecnt > GENERALFIRERATE) {
			Fire(general);
		}
		if (0.5 >= (float)hp / (float)TANKHP) {
			if (guidedfirecnt > GUIDEDFIRERATE) {
				Fire(guided);
			}
		}
		if(antifirecnt > ANTIFIRERATE) {
			for (size_t t = 0; t < target->bvec.size(); t++) {
				if (target->bvec[t].bulltype == guided) {
					Fire(anti_guided);
				}		
			}
		}
		// ---Fire

		// Move
		if (curstate != evade) {
			if (target->axis.x < lineLastAxis.x) {
				if (M_PI + M_PI / 4 - 0.05f <= angle && angle <= M_PI + M_PI / 4 + 0.05f) {
					MoveController(-1);
				}
				else if (M_PI + M_PI / 4 > angle) {
					AI_Aim(+1);
				}
				else if (M_PI + M_PI / 4 < angle) {
					AI_Aim(-1);
				}
			}
			else if (target->axis.x > lineLastAxis.x) {
				if (M_PI * 2 - M_PI / 4 - 0.05f <= angle && angle <= M_PI * 2 - M_PI / 4 + 0.05f) {
					MoveController(+1);
				}
				else if (M_PI * 2 - M_PI / 4 > angle) {
					AI_Aim(+1);
				}
				else if (M_PI * 2 - M_PI / 4 < angle) {
					AI_Aim(-1);
				}
			}
		}
		// ---Move


		// Evade 
		blitr bitr = target->bvec.begin();
		blitr eitr = target->bvec.end();
		for (; bitr != eitr; bitr++) {
			float a = bitr->axis.x - axis.x;
			float b = bitr->axis.y - axis.y;
			float h = sqrtf(powf(a, 2) + powf(b, 2));
			if (h < 100) {
				curstate = evade;
				if (a < 0) {
					MoveController(+1);
				}
				else if (a > 0) {
					MoveController(-1);
				}
			}
			else {
				curstate = idle;
			}
		}

		float a = target->axis.x - axis.x;
		float b = target->axis.y - axis.y;
		float h = sqrtf(powf(a, 2) + powf(b, 2));

		if (h < 150) {
			if (axis.x + 5 >= WINSIZEX - TANKSIZE) {
				lockdirection = true;
				lockdirtemper = -1;
			}
			else if (axis.x - 5 <= TANKSIZE) {
				lockdirection = true;
				lockdirtemper = 1;
			}
			else {
				if (a < 0) {
					MoveController(1);
				}
				else if (a > 0) {
					MoveController(-1);
				}
			}
		}

		if (lockdirection && lockdirtemper != 0) {
			if (lockdirtemper == -1) {
				if (axis.x + 200 <= target->axis.x) {
					lockdirection = false;
				}
			}
			else if (lockdirtemper == 1) {
				if (axis.x - 200 >= target->axis.x) {
					lockdirection = false;
				}
			}
			MoveController(lockdirtemper);
		}
		// ---Evade
	}
	void AI_Aim(int x) {
		angle += AIMSPD * x;
		DrawLine();
	}
	void AI_Evade(int x) {
		axis.x += x * EVADESPD;
		DrawLine();
	}
	/// ---AI Controller

	/// Draw
	void Draw(HDC hdc) {
		RectangleMakeCenter(hdc, axis.x, axis.y, TANKSIZE*2, TANKSIZE);
		LineMake(hdc, axis.x, axis.y, axis.x + cosf(angle)*50, axis.y + sinf(angle)*50);
		
		blitr bitr;
		blitr eitr;
		bitr = bvec.begin();
		eitr = bvec.end();
		
		for (; bitr != eitr; bitr++) {
			EllipseMakeCenter(hdc, bitr->axis.x, bitr->axis.y, bitr->size, bitr->size);
		}
		
		if(controlType != ai){
			bitr = linevec.begin();
			eitr = linevec.end();
			HBRUSH lbrush = CreateSolidBrush(RGB(_lr, _lg, _lb));
			HBRUSH oldlBrush = (HBRUSH)SelectObject(hdc, lbrush);
			for (; bitr != eitr; bitr++) {
				EllipseMakeCenter(hdc, bitr->axis.x, bitr->axis.y, LINESIZE, LINESIZE);
			}
			SelectObject(hdc, oldlBrush);
			DeleteObject(lbrush);
		}

		MakeUI();
		
		Rectangle(hdc, NameTag);
		if (controlType == player) {
			TextOut(hdc, axis.x-30, axis.y + 53, "PLAYER", strlen("PLAYER"));
		}
		else if (controlType == ai) {
			TextOut(hdc, axis.x- 5, axis.y + 53, "AI", strlen("AI"));
		}
		Rectangle(hdc, hpBarUnder);
		HBRUSH brush = CreateSolidBrush(RGB(_r, _g, _b));            // 플레이어 색
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, hpBar);
		SelectObject(hdc, oldBrush);
		DeleteObject(brush);
	}
	void DrawLine() {
		linevec.clear();
		float curGravity = 0;
		POINTFLOAT tempAxis;
		lockon = false;
		tempAxis.x = axis.x + cosf(angle) * 50;
		tempAxis.y = axis.y + sinf(angle) * 50;
		_lr = 200;
		_lg = 200;
		_lb = 50;

		while (true) {
			if (tempAxis.x < 0 || tempAxis.y < 0 || tempAxis.x > WINSIZEX || tempAxis.y > WINSIZEY-100) {
				break;
			}
		
			Bullet<Tank>* newBullet = new Bullet<Tank>();
			tempAxis.x = tempAxis.x + cosf(angle) * GENERALSPEED;
			tempAxis.y = tempAxis.y + sinf(angle) * GENERALSPEED;
			tempAxis.y += curGravity;
		
			newBullet->axis.x = tempAxis.x;
			newBullet->axis.y = tempAxis.y;

		

			linevec.push_back(*newBullet);

			curGravity += GRAVITY;

			RECT temprect;
			RECT temprect1;
			RECT temprect2;
			temprect1 = RectMakeCenter(tempAxis.x, tempAxis.y, 5, 5);
			temprect2 = target->collision;
			if (IntersectRect(&temprect, &temprect1, &temprect2)) {
				lockon = true;
				_lr = 50;
				_lg = 200;
				_lb = 50;
			}
		}
		lineLastAxis.x = tempAxis.x;
		lineLastAxis.y = tempAxis.y;
	}
	void MakeUI() {
		hpBar = RectMakeCenter(axis.x, axis.y + 30, HPBARSIZE, 10);
		hpBarUnder = RectMakeCenter(axis.x, axis.y + 30, HPBARSIZE + 10, 20);
		NameTag = RectMakeCenter(axis.x, axis.y + 60, 80, 20);
		float hprate =  (float)hp / (float)TANKHP;
		hpBar.right -= HPBARSIZE * (float)(1- hprate);

		if (hpBar.right <= hpBar.left) {
			hpBar.right = hpBar.left + 1;
		}
		
		if (hprate > 0.5) {
			_r = 50;
			_g = 200;
			_b = 50;
		}
		else if (hprate >= 0.3) {
			_r = 200;
			_g = 200;
			_b = 50;
		}
		else if (hprate < 0.3) {
			_r = 200;
			_g = 50;
			_b = 50;
		}
	}
	/// ---Draw

	/// Update
	void MakeCollision() {
		collision = RectMake(axis.x, axis.y, TANKSIZE * 2 + 5, TANKSIZE + 5);
	}
	void DamageControl(Tank& target) {
		RECT temprect;
		blitr pointitr;
		Tank a;
		int tempradius;

		for (size_t t = 0; t < bvec.size(); t++) {
			switch (bvec[t].bulltype) {
			case general:
				tempradius = GENERALSIZE / 2;
				break;
			case guided:
				tempradius = GUIDEDSIZE / 2;
				break;
			case anti_guided:
				tempradius = ANTISIZE / 2;
				break;
			}

			if (bvec[t].bulltype != anti_guided) {
				if (bvec[t].axis.x >= target.collision.left - tempradius &&
					bvec[t].axis.x <= target.collision.right + tempradius) {
					if (bvec[t].axis.y >= target.collision.top - tempradius &&
						bvec[t].axis.y <= target.collision.bottom + tempradius) {
						target.hp -= bvec[t].damage;
						bvec[t].amIDestroyed = true;
					}
				}
			}

			else if (bvec[t].bulltype == anti_guided && bvec[t].locked) {
				float dx = bvec[t].axis.x - bvec[t].targetBullet->axis.x;
				float dy = bvec[t].axis.y - bvec[t].targetBullet->axis.y;
				float h = sqrtf(powf(dx, 2) + powf(dy, 2));
				if (ANTISIZE + GUIDEDSIZE >= h) {
					bvec[t].targetBullet->amIDestroyed = true;
					bvec[t].amIDestroyed = true;
				}
			}
		}
	}
	void BUlletUpdate() {
		blitr pointitr;
		for (size_t t = 0; t < bvec.size();) {
			if (bvec[t].amIDestroyed) {
				pointitr = bvec.begin() + t;
				bvec.erase(pointitr);
			}
			else {
				bvec[t].BulletMove();
				t++;
			}
		}
	}
	void TankUpdate() {
		generalfirecnt++;
		guidedfirecnt++;
		antifirecnt++;
		Controller();
		MakeCollision();
		DamageControl(*target);
		BUlletUpdate();
	}
	///---Update
};


class playGround : public gameNode
{
private:
	Tank* P1;
	Tank* AI;
	typedef vector<Bullet<Tank>>::iterator blitr;


public:
	playGround();
	~playGround();

	virtual HRESULT init();			//초기화 전용 함수
	virtual void release();			//메모리 해제 함수
	virtual void update();			//연산 전용 함수
	virtual void render(HDC hdc);	//그리기 전용 함수
};