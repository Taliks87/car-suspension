// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "pawn_car.generated.h"

class USuspension;

UCLASS(Blueprintable)
class CAR_API APawnCar : public APawn
{
	GENERATED_BODY()

public:
	APawnCar();
protected:
	void BeginPlay() override;
public:
	void Tick(float DeltaTime) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	void CameraYaw(float Axis);
	void TurnWheel(float Axis);
	void MoveCar(float Axis);
	void CameraPitch(float Axis);
	void CameraZoom(float Axis);
	void CamMoveFront();
	void CamMoveBack();

protected:
	UPROPERTY(EditAnywhere, Category = "Car data")
		UStaticMeshComponent* MeshBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		float Mass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		FVector WeightDistribution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		USuspension* FrontSuspensions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		USuspension* RearSuspensions;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;

	UPROPERTY()
		FVector2D CameraAxis;
};
