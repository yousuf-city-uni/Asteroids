#include <stdlib.h>
#include "GameUtil.h"
#include "Cog.h"
#include "BoundingShape.h"

Cog::Cog(void) : GameObject("Cog")
{
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
}

Cog::~Cog(void)
{
}

bool Cog::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Cog::OnCollision(const GameObjectList& objects)
{
	for (const auto& obj : objects)
	{
		if (obj->GetType() == GameObjectType("Spaceship")) {
			mWorld->FlagForRemoval(GetThisPtr());
		}
	}

}
