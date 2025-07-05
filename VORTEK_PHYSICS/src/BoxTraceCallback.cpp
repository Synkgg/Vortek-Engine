#include "Physics/BoxTraceCallback.h"

namespace VORTEK_PHYSICS
{
bool BoxTraceCallback::ReportFixture( b2Fixture* pFixture )
{
	m_Bodies.push_back( pFixture->GetBody() );

	return true;
}
} // namespace VORTEK_PHYSICS
