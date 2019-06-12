// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "suspension.generated.h"

class USuspensionSide;
using FuncForce = std::function<void(FVector, FVector, FName)>;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAR_API USuspension : public USceneComponent
{	
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USuspension();

	void init(const FuncForce& funcAddForceAtbody);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
	float trackWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
	float relaxDamperLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
	float springMove;
	//stiffness can by calculate (G*r^4)/(4*n*R^3)
	//G - shear modulus (mPa) 78500 mPa
	//n - number of turns 5
	//r - bar radius (mm) 6 mm
	//R - coil radius (mm) 66 mm
	//stiffness - H/mm (for UE4 need convert H/sm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
	float stiffness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
	float damper;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
	float wheelRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
	float wheelWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension data")
	float kpiAngle;

	UPROPERTY(EditAnywhere)
	USuspensionSide* leftBlock;
	/*
	UPROPERTY(EditAnywhere)
	USuspensionSide* right;
*/
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:		
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
	void refreshBlock(float DeltaTime, USuspensionSide* suspSide);
private:
	FuncForce addForceAtbody;
};
