#include "Physics/Box2DWrappers.h"

namespace Vortek::Physics
{
void BodyDestroyer::operator()( b2Body* body ) const
{
	body->GetWorld()->DestroyBody( body );
}
} // namespace Vortek::Physics
