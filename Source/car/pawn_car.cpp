// Fill out your copyright notice in the Description page of Project Settings.
#include "pawn_car.h"
#include "Engine.h"

#include "suspension.h"
#include <array>

// Sets default values
APawnCar::APawnCar()
	: mesh_body()
	, mass(1300.0f)
	, weightDistribution(0.3f, 0.1f)
	, frontSuspensions()
	, rearSuspensions()
	, springArm()
	, camera()	
{
 	PrimaryActorTick.bCanEverTick = true;
	mesh_body = CreateDefaultSubobject<UStaticMeshComponent>("mesh_body");		
	frontSuspensions = CreateDefaultSubobject<USuspension>("front_suspension");
	rearSuspensions = CreateDefaultSubobject<USuspension>("rear_suspension");
		
	RootComponent = mesh_body;
	mesh_body->SetSimulatePhysics(true);
	frontSuspensions->SetupAttachment(mesh_body);
	rearSuspensions->SetupAttachment(mesh_body);	
	
	frontSuspensions->SetRelativeLocation({ 0.0f, -1800.0f, -730.0f });
	rearSuspensions->SetRelativeLocation({ 0.0f, 1950.0f, -730.0f });

	//Init camera
	springArm = CreateDefaultSubobject<USpringArmComponent>("spring_arm_camera");
	camera = CreateDefaultSubobject<UCameraComponent>("camera");
	springArm->TargetArmLength = 4000.0f;
	springArm->SetupAttachment(mesh_body);
	camera->SetupAttachment(springArm);
	
	springArm->bEnableCameraLag = true;
	springArm->CameraLagSpeed = 6.0f;
	springArm->bDrawDebugLagMarkers = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APawnCar::BeginPlay()
{	
	mesh_body->SetMassOverrideInKg(NAME_None, mass);	
	mesh_body->SetCenterOfMass({ weightDistribution.X , weightDistribution.Y, 0.0f });
	auto f = std::bind(&UStaticMeshComponent::AddForceAtLocation, mesh_body, std::placeholders::_1, std::placeholders::_2, NAME_None);
	frontSuspensions->Init(mass, f);
	rearSuspensions->Init(mass, f);
	Super::BeginPlay();		
}

void APawnCar::Tick(float DeltaTime)
{	
	GEngine->ClearOnScreenDebugMessages();	
	Super::Tick(DeltaTime);

	springArm->AddRelativeRotation({ cameraAxis.Y, cameraAxis.X, 0.0f });	
}

// Called to bind functionality to input
void APawnCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	Super::SetupPlayerInputComponent(PlayerInputComponent);	
	InputComponent->BindAction("cam_move_left", IE_Pressed, this, &APawnCar::CamMoveLeft);
	InputComponent->BindAction("cam_move_right", IE_Pressed, this, &APawnCar::CamMoveReight);
	InputComponent->BindAxis("turn_left", this, &APawnCar::TurnWheel);
	InputComponent->BindAxis("turn_right", this, &APawnCar::TurnWheel);
	InputComponent->BindAxis("move_front", this, &APawnCar::MoveCar);
	InputComponent->BindAxis("move_back", this, &APawnCar::MoveCar);
	InputComponent->BindAxis("CameraYaw", this, &APawnCar::CameraYaw);
	InputComponent->BindAxis("CameraPitch", this, &APawnCar::CameraPitch);
}

void APawnCar::TurnWheel(float axis)
{
	frontSuspensions->TurnWheel(axis);
}

void APawnCar::MoveCar(float axis)
{	
	//simple move
	FVector v = GetActorRotation().RotateVector({ 0.0f, -500.0f * axis, 0.0f });	
	mesh_body->AddForce(v, NAME_None, true);
}

void APawnCar::CameraYaw(float axis)
{
	cameraAxis.X = axis;
}

void APawnCar::CameraPitch(float axis)
{
	cameraAxis.Y = axis;
}

void APawnCar::CamMoveLeft()
{
	springArm->AddRelativeLocation({ 0.0f, -100.0f, 0.0f });
}

void APawnCar::CamMoveReight()
{
	springArm->AddRelativeLocation({ 0.0f, 100.0f, 0.0f });
}