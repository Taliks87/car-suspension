#pragma once
#include <functional>
#include <memory>


#include "CoreMinimal.h"

namespace tools
{
	using FFuncForce = std::function<void(FVector, FVector, FName)>;
	struct FCommonSuspensionData {

		FCommonSuspensionData(float _CommonMass, float _RelaxDamperLength, float _DamperMove, float _Stiffness, float _Damper,
			float _WheelRadius, float _WheelWidth, float _WheelMass, float _KpiAngle, float _FrictionKof, const tools::FFuncForce& _AddForceAtBody);

		float CommonMass;
		float RelaxDamperLength;
		float DamperMove;
		//stiffness can by calculate (G*r^4)/(4*n*R^3)
		//G - shear modulus (mPa) 78500 mPa
		//n - number of turns 5
		//r - bar radius (mm) 6 mm
		//R - coil radius (mm) 66 mm
		//stiffness - H/mm (for UE4 need convert H/sm)
		float Stiffness; // Now is set by user
		float Damper;
		float WheelRadius;
		float WheelWidth;
		float WheelMass;
		float KpiAngle;
		//frictionKof depend of surface
		float FrictionKof;
		tools::FFuncForce AddForceAtBody;		
	};
	using FCommonSuspensionDataPtr = std::shared_ptr<FCommonSuspensionData>;
}