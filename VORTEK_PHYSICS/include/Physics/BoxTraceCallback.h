#pragma once

#include <box2d/box2d.h>
#include <vector>

namespace VORTEK_PHYSICS
{
class BoxTraceCallback : public b2QueryCallback
{
  public:
	BoxTraceCallback() = default;

	virtual bool ReportFixture( b2Fixture* pFixture ) override;
	std::vector<b2Body*>& GetBodies() { return m_Bodies; }

  private:
	std::vector<b2Body*> m_Bodies;
};
} // namespace VORTEK_PHYSICS
