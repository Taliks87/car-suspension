#pragma once
#include <functional>
#include <memory>


#include "CoreMinimal.h"

namespace tools
{
	using FuncForce = std::function<void(FVector, FVector, FName)>;
	struct CommonSuspensionData {

		CommonSuspensionData(float _commonMass, float _relaxDamperLength, float _damperMove, float _stiffness, float _damper,
			float _wheelRadius, float _wheelWidth, float _kpiAngle, float _frictionKof, const tools::FuncForce& _addForceAtBody);

		float commonMass;
		float relaxDamperLength;
		float damperMove;
		//stiffness can by calculate (G*r^4)/(4*n*R^3)
		//G - shear modulus (mPa) 78500 mPa
		//n - number of turns 5
		//r - bar radius (mm) 6 mm
		//R - coil radius (mm) 66 mm
		//stiffness - H/mm (for UE4 need convert H/sm)
		float stiffness; // Now is set by user
		float damper;
		float wheelRadius;
		float wheelWidth;
		float kpiAngle;
		//frictionKof depend of surface
		float frictionKof;
		tools::FuncForce addForceAtBody;		
	};
	using CommonSuspensionDataPtr = std::shared_ptr<CommonSuspensionData>;
}