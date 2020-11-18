// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CarPawn.generated.h"

class USuspensionComponent;

UCLASS(Blueprintable)
class CAR_API ACarPawn : public APawn
{
	GENERATED_BODY()

public:
	ACarPawn();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	virtual	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
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
	UStaticMeshComponent* MeshBodyComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
	float Mass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
	FVector WeightDistribution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
	USuspensionComponent* FrontSuspensionsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
	USuspensionComponent* RearSuspensionsComponent;

	UPROPERTY()
	USpringArmComponent* SpringArmComponent;

	UPROPERTY()
	UCameraComponent* CameraComponent;

	UPROPERTY()
	FVector2D CameraAxis;
};
