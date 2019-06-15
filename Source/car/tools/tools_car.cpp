#include "tools_car.h"

namespace tools
{
	SuspensionData::SuspensionData(float _relaxDamperLength, float _damperMove, float _stiffness, float _damper,
		float _wheelRadius, float _wheelWidth, float _kpiAngle, const tools::FuncForce& _addForceAtBody)
		: relaxDamperLength(_relaxDamperLength)
		, damperMove(_damperMove)
		, stiffness(_stiffness)
		, damper(_damper)
		, wheelRadius(_wheelRadius)
		, wheelWidth(_wheelWidth)
		, kpiAngle(_kpiAngle)
		, addForceAtBody(_addForceAtBody)
	{

	}
}