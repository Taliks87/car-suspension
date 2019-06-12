#include "tools_car.h"

namespace tools
{
	SuspensionData::SuspensionData(float relaxDamperLength, float springMove, float stiffness, float damper,
		float wheelRadius, float wheelWidth, float kpiAngle, const tools::FuncForce& addForceAtBody)
		: relaxDamperLength(relaxDamperLength)
		, springMove(springMove)
		, stiffness(stiffness)
		, damper(damper)
		, wheelRadius(wheelRadius)
		, wheelWidth(wheelWidth)
		, kpiAngle(kpiAngle)
		, addForceAtBody(addForceAtBody)
	{

	}
}