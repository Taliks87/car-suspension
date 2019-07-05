// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "pawn_car.generated.h"

class USuspension;

UCLASS()
class CAR_API APawnCar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawnCar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:	
	void CameraYaw(float Axis);
	void TurnWheel(float Axis);	
	void MoveCar(float Axis);
	void CameraPitch(float Axis);
	void CamMoveLeft();
	void CamMoveReight();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Car data")
		UStaticMeshComponent* MeshBody;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		float Mass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		FVector2D WeightDistribution;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		USuspension* FrontSuspensions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		USuspension* RearSuspensions;	
	UPROPERTY(EditAnywhere)
		USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;	

	FVector2D CameraAxis;
};
