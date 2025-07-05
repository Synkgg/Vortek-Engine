#include "Physics/Box2DWrappers.h"

namespace VORTEK_PHYSICS
{
void BodyDestroyer::operator()( b2Body* body ) const
{
	body->GetWorld()->DestroyBody( body );
}
} // namespace VORTEK_PHYSICS
