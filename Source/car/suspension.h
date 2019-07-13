// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "tools/tools_car.h"

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "suspension.generated.h"

class USuspensionSide;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAR_API USuspension : public USceneComponent
{	
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USuspension();
	void Init(float Mass, const tools::FFuncForce& FuncAddForceAtbody);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		

	void TurnWheel(float Axis);	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float TrackWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float RelaxDamperLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float DamperMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float Stiffness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float Damper;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float WheelRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float WheelWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float WheelMass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float KpiAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float FrictionKof;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		USuspensionSide* LeftBlock;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		USuspensionSide* RightBlock;
private:
	float MaxTurnAngle;
	float CurrTurnAngle;
};
