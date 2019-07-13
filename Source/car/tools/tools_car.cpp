#include "tools_car.h"

namespace tools
{
	FCommonSuspensionData::FCommonSuspensionData(float _CommonMass, float _RelaxDamperLength, float _DamperMove, float _Stiffness, float _Damper,
		float _WheelRadius, float _WheelWidth, float _WheelMass, float _KpiAngle, float _FrictionKof, const tools::FFuncForce& _AddForceAtBody)
		: CommonMass(_CommonMass)
		, RelaxDamperLength(_RelaxDamperLength)
		, DamperMove(_DamperMove)
		, Stiffness(_Stiffness)
		, Damper(_Damper)
		, WheelRadius(_WheelRadius)
		, WheelWidth(_WheelWidth)
		, WheelMass(_WheelMass)
		, KpiAngle(_KpiAngle)
		, FrictionKof(_FrictionKof)
		, AddForceAtBody(_AddForceAtBody)
	{

	}
}