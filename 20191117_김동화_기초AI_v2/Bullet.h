#pragma once
#include "stdafx.h"
class Bullet
{
public:
	Bullet();
	~Bullet();
};

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
	reload
};

enum Move_direction {
	left,
	right
};

class Bullet {
public:
	POINTFLOAT axis;
	float angle;
	float speed;
	float gravity;
	float damage;
	float size;
	struct Tank* target;
	Bullet* targetBullet;
	bool locked;
	RECT collision;
	Bullet_Type bulltype;





	virtual void BulletMove() {
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
					}
					break;
				}
				axis.x += cosf(angle)*speed;
				axis.y += sinf(angle)*speed;
			}
			break;
		}

		MakeCollision();
	}
	virtual void MakeCollision() {
		collision = RectMake(axis.x, axis.y, size, size);
	}


	Bullet();
	~Bullet();

};
