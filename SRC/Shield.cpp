#include <stdlib.h>
#include "GameUtil.h"
#include "Shield.h"
#include "BoundingShape.h"

Shield::Shield(void) : GameObject("Shield")
{
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
}

Shield::~Shield(void)
{
}

void Shield::SetPosition(GLVector3f a)
{
	mPosition.x = a[0];
	mPosition.y = a[1];
}

bool Shield::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Shield::OnCollision(const GameObjectList& objects)
{
	for (const auto& obj : objects)
	{
		if (obj->GetType() == GameObjectType("Asteroid")) {
			return;
		}
		if (obj->GetType() == GameObjectType("Spaceship")) {
			SetPosition(obj->GetPosition());
			mWorld->FlagForRemoval(GetThisPtr());
		}
	}

}
