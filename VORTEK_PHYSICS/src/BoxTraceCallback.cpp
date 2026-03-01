#include "Physics/BoxTraceCallback.h"

namespace Vortek::Physics
{
bool BoxTraceCallback::ReportFixture( b2Fixture* pFixture )
{
	m_Bodies.push_back( pFixture->GetBody() );

	return true;
}
} // namespace Vortek::Physics
