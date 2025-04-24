#ifndef __LIFE_H__
#define __LIFE_H__

#include "GameObject.h"

class Life : public GameObject
{
public:
	Life(void);
	~Life(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif
