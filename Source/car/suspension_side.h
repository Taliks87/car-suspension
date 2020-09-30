#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "suspension_side.generated.h"

using FFuncForce = std::function<void(FVector, FVector, FName)>;

USTRUCT()
struct FCommonSuspensionData {
	GENERATED_BODY()

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
	float WheelSpringiness;
	float KpiAngle;	
	//frictionKof depend of surface
	float FrictionKof;
	FFuncForce AddForceAtBody;
};
using FCommonSuspensionDataPtr = TSharedPtr<FCommonSuspensionData>;


UCLASS(ClassGroup = (Custom))
class CAR_API USuspensionSide : public USceneComponent
{
	GENERATED_BODY()

public:
	USuspensionSide();
	void Init(const FCommonSuspensionDataPtr& newSuspensionData, bool isLeftSide);
protected:
	void BeginPlay() override;
public:
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TurnWheel(float axis);
private:
	void UpdateSuspension(float DeltaTime);
	void UpdateForceOnWheel(float SuspensionForce, float DeltaTime);

protected:
	UPROPERTY(EditAnywhere, Category = "Block data")
		UMeshComponent* MeshWheel;

	UPROPERTY()
		USceneComponent* SceneWheelCenter;

	UPROPERTY()
		USceneComponent* SceneDamperPointTop;

	UPROPERTY()
		USceneComponent* SceneDamperPointBot;
private:
	bool bIsLeft;
	float SpringForce;
	float DamperForce;
	float SuspensionForce;
	float CompressionVelocity;
	float WheelSpinVelocity;
	float ReyLength;
	float CurrDamperLength;
	float MaxDamperLength;
	float MinDamperLength;
	FCommonSuspensionDataPtr Data;

	FCollisionQueryParams CollisionParams;
};
