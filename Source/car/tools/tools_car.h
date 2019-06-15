#pragma once
#include <functional>
#include <memory>

#include "CoreMinimal.h"

namespace tools
{
	using FuncForce = std::function<void(FVector, FVector, FName)>;
	struct SuspensionData {

		SuspensionData(float relaxDamperLength, float _damperMove, float _stiffness, float _damper,
			float _wheelRadius, float _wheelWidth, float _kpiAngle, const tools::FuncForce& _addForceAtBody);

		float relaxDamperLength;
		float damperMove;
		float stiffness;
		float damper;
		float wheelRadius;
		float wheelWidth;
		float kpiAngle;
		tools::FuncForce addForceAtBody;
	};
	using SuspensionDataPtr = std::shared_ptr<SuspensionData>;
}