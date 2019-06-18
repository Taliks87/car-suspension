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
	void Init(tools::CommonSuspensionDataPtr& newSuspensionData, bool isLeftSide);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

	void turnWheel(float axis);	
private:
	void RefreshBlock(float DeltaTime);
	void addFrictionForce(float suspensionForce, const FVector& hitPos);

protected:
	UPROPERTY(EditAnywhere, Category = "Block data")
	UStaticMeshComponent* mesh_wheel;		
protected:	
	USceneComponent* scene_wheelCenter;
	USceneComponent* scene_damperPointTop;
	USceneComponent* scene_damperPointBot;
private:	
	bool isLeft;
	float springForce;	
	float damperForce;	
	float suspensionForce;
	float suspensionSpeed;
	float reyLength;		
	float currDamperLength;	
	float maxDamperLength;		
	float minDamperLength;		
	
	tools::CommonSuspensionDataPtr data;

	FCollisionQueryParams collisionParams;
};
