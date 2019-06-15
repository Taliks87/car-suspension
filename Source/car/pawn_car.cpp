// Fill out your copyright notice in the Description page of Project Settings.
#include "pawn_car.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"

#include "suspension.h"
#include <array>

// Sets default values
APawnCar::APawnCar()
	: mesh_body()
	, mass(1300.0f)
	, weightDistribution(0.7f)
	, frontSuspensions()
	, rearSuspensions()
	//, springArmCamera()
	//, camera()
{
 	PrimaryActorTick.bCanEverTick = true;
	mesh_body = CreateDefaultSubobject<UStaticMeshComponent>("mesh_body");
	RootComponent = mesh_body;	
	frontSuspensions = CreateDefaultSubobject<USuspension>("front_suspension");
	rearSuspensions = CreateDefaultSubobject<USuspension>("rear_suspension");
		
	frontSuspensions->SetupAttachment(mesh_body);
	rearSuspensions->SetupAttachment(mesh_body);	
	
	frontSuspensions->SetRelativeLocation({ 0.0f, -1800.0f, -730.0f });
	rearSuspensions->SetRelativeLocation({ 0.0f, 1950.0f, -730.0f });
	auto f = std::bind(&UStaticMeshComponent::AddForceAtLocation, mesh_body, std::placeholders::_1, std::placeholders::_2, NAME_None);
	frontSuspensions->Init(f);
	rearSuspensions->Init(f);
	//Init camera
	USpringArmComponent* springArmCamera = CreateDefaultSubobject<USpringArmComponent>("spring_arm_camera");
	UCameraComponent* camera = CreateDefaultSubobject<UCameraComponent>("camera");
	springArmCamera->TargetArmLength = 4000.0f;
	springArmCamera->SetupAttachment(mesh_body);
	camera->SetupAttachment(springArmCamera);
	/*springArmCamera->AttachToComponent(mesh_body, FAttachmentTransformRules::SnapToTargetIncludingScale);
	camera->AttachToComponent(springArmCamera, FAttachmentTransformRules::SnapToTargetIncludingScale);*/

	
	//springArmCamera->SetRelativeRotation({0.0f, 0.0f, 0.0f});	
	springArmCamera->bEnableCameraLag = true;
	springArmCamera->CameraLagSpeed = 6.0f;
	springArmCamera->bDrawDebugLagMarkers = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APawnCar::BeginPlay()
{		
	//mesh_body->SetCenterOfMass()
	//mesh_body->SetMassScale();
	Super::BeginPlay();		
}

void APawnCar::Tick(float DeltaTime)
{
/*	for (TObjectIterator<USpringArmComponent> itr; itr; ++itr) {
		if (itr->GetName() == "spring_arm_camera") {
			USpringArmComponent* springArmCamera = *itr;
			springArmCamera->TargetArmLength = 4000.0f;
			springArmCamera->AttachToComponent(mesh_body, FAttachmentTransformRules::SnapToTargetIncludingScale);			
			for (TObjectIterator<UCameraComponent> itr; itr; ++itr) {
				if (itr->GetName() == "camera") {
					UCameraComponent* camera = *itr;
					camera->AttachToComponent(springArmCamera, FAttachmentTransformRules::SnapToTargetIncludingScale);
					break;
				}				
			}
			break;
		}
	}*/	
	
	GEngine->ClearOnScreenDebugMessages();	
	frontSuspensions->TickComponent(DeltaTime, LEVELTICK_All, nullptr);
	rearSuspensions->TickComponent(DeltaTime, LEVELTICK_All, nullptr);
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APawnCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	Super::SetupPlayerInputComponent(PlayerInputComponent);	
	InputComponent->BindAction("cam_move_left", IE_Pressed, this, &APawnCar::camMoveLeft);
	InputComponent->BindAction("cam_move_right", IE_Pressed, this, &APawnCar::camMoveReight);
	InputComponent->BindAxis("turn_left", this, &APawnCar::turnWheel);
	InputComponent->BindAxis("turn_right", this, &APawnCar::turnWheel);
}

void APawnCar::turnWheel(float axis)
{
	frontSuspensions->turnWheel(axis);
}

void APawnCar::camMoveLeft()
{
	for (TObjectIterator<USpringArmComponent> itr; itr; ++itr){		
		if(itr->GetName() == "spring_arm_camera"){
			USpringArmComponent* springArmCamera = *itr;
			springArmCamera->AddRelativeLocation({ 0.0f, -100.0f, 0.0f });
		}
	}	
}

void APawnCar::camMoveReight()
{
	for (TObjectIterator<USpringArmComponent> itr; itr; ++itr)
	{
		// Access the subclass instance with the * or -> operators.		
		if (itr->GetName() == "spring_arm_camera")
		{
			USpringArmComponent* springArmCamera = *itr;
			springArmCamera->AddRelativeLocation({ 0.0f, 100.0f, 0.0f });
		}
	}
}