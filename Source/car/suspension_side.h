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
	void Init(tools::SuspensionDataPtr& newSuspensionData, bool isLeftSide);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
private:
	void RefreshBlock(float DeltaTime);

public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh_wheel;		

protected:	
	USceneComponent* scene_wheelCenter;
	USceneComponent* scene_damperPointTop;
	USceneComponent* scene_damperPointBot;
	USceneComponent* scene_botPoint;		
private:	
	bool isLeft;
	float springForce;	
	float reyLength;		
	float currDamperLength;
	float oldDamperLength;
	tools::SuspensionDataPtr data;
};