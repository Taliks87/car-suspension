#include "tools_car.h"

namespace tools
{
	CommonSuspensionData::CommonSuspensionData(float _commonMass, float _relaxDamperLength, float _damperMove, float _stiffness, float _damper,
		float _wheelRadius, float _wheelWidth, float _kpiAngle, float _frictionKof, const tools::FuncForce& _addForceAtBody)
		: commonMass(_commonMass)
		, relaxDamperLength(_relaxDamperLength)
		, damperMove(_damperMove)
		, stiffness(_stiffness)
		, damper(_damper)
		, wheelRadius(_wheelRadius)
		, wheelWidth(_wheelWidth)
		, kpiAngle(_kpiAngle)
		, frictionKof(_frictionKof)
		, addForceAtBody(_addForceAtBody)
	{

	}
}