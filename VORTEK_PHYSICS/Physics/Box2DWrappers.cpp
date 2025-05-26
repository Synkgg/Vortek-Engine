#include "Box2DWrappers.h"

void VORTEK_PHYSICS::BodyDestroyer::operator()(b2Body* body) const
{
    body->GetWorld()->DestroyBody(body);
}
