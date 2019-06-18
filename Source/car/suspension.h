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
	void Init(float mass, const tools::FuncForce& funcAddForceAtbody);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		

	void turnWheel(float axis);	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float trackWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float relaxDamperLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float damperMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float stiffness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float damper;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float wheelRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float wheelWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float wheelMass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float kpiAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		float frictionKof;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		USuspensionSide* leftBlock;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
		USuspensionSide* rightBlock;
private:
	float maxTurnAngle;
	float currTurnAngle;
};
