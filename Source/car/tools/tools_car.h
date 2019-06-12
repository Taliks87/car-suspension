#pragma once
#include <functional>
#include <memory>

#include "CoreMinimal.h"

namespace tools
{
	using FuncForce = std::function<void(FVector, FVector, FName)>;
	struct SuspensionData {

		SuspensionData(float relaxDamperLength, float springMove, float stiffness, float damper, 
			float wheelRadius, float wheelWidth, float kpiAngle, const tools::FuncForce& addForceAtBody);

		float relaxDamperLength;
		float springMove;
		float stiffness;
		float damper;
		float wheelRadius;
		float wheelWidth;
		float kpiAngle;
		tools::FuncForce addForceAtBody;
	};
	using SuspensionDataPtr = std::shared_ptr<SuspensionData>;
}