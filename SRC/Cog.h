#ifndef __COG_H__
#define __COG_H__

#include "GameObject.h"

class Cog : public GameObject
{
public:
	Cog(void);
	~Cog(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif
