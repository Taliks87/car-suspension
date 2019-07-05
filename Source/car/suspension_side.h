// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "tools/tools_car.h"

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "suspension_side.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAR_API USuspensionSide : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USuspensionSide();
	void Init(tools::FCommonSuspensionDataPtr& newSuspensionData, bool isLeftSide);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

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
	
	tools::FCommonSuspensionDataPtr Data;

	FCollisionQueryParams CollisionParams;
};
