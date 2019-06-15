// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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
	void turnWheel(float axis);	
	void camMoveLeft();
	void camMoveReight();

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* mesh_body;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		float mass;//kg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		float weightDistribution;//kofficent	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		USuspension* frontSuspensions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car data")
		USuspension* rearSuspensions;	
	//UPROPERTY(EditAnywhere)
	//	USpringArmComponent* springArm;
	//UCameraComponent* camera;
};
