// Fill out your copyright notice in the Description page of Project Settings.
#include "pawn_car.h"
#include "Engine.h"

#include "suspension.h"
#include <array>

// Sets default values
APawnCar::APawnCar()
	: MeshBody()
	, Mass(1300.0f)
	, WeightDistribution(0.3f, 0.1f)
	, FrontSuspensions()
	, RearSuspensions()
	, SpringArm()
	, Camera()	
{
 	PrimaryActorTick.bCanEverTick = true;
	MeshBody = CreateDefaultSubobject<UStaticMeshComponent>("mesh_body");		
	FrontSuspensions = CreateDefaultSubobject<USuspension>("front_suspension");
	RearSuspensions = CreateDefaultSubobject<USuspension>("rear_suspension");
		
	RootComponent = MeshBody;
	MeshBody->SetSimulatePhysics(true);
	FrontSuspensions->SetupAttachment(MeshBody);
	RearSuspensions->SetupAttachment(MeshBody);	
	
	FrontSuspensions->SetRelativeLocation({ 0.0f, -1800.0f, -730.0f });
	RearSuspensions->SetRelativeLocation({ 0.0f, 1950.0f, -730.0f });

	//Init camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("spring_arm_camera");
	Camera = CreateDefaultSubobject<UCameraComponent>("camera");
	SpringArm->TargetArmLength = 4000.0f;
	SpringArm->SetupAttachment(MeshBody);
	Camera->SetupAttachment(SpringArm);
	
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 6.0f;
	SpringArm->bDrawDebugLagMarkers = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APawnCar::BeginPlay()
{	
	MeshBody->SetMassOverrideInKg(NAME_None, Mass);	
	MeshBody->SetCenterOfMass({ WeightDistribution.X , WeightDistribution.Y, 0.0f });
	auto FuncAddForceAtbody = std::bind(&UStaticMeshComponent::AddForceAtLocation, MeshBody, std::placeholders::_1, std::placeholders::_2, NAME_None);
	FrontSuspensions->Init(Mass, FuncAddForceAtbody);
	RearSuspensions->Init(Mass, FuncAddForceAtbody);
	Super::BeginPlay();		
}

void APawnCar::Tick(float DeltaTime)
{	
	GEngine->ClearOnScreenDebugMessages();	
	Super::Tick(DeltaTime);

	SpringArm->AddRelativeRotation({ CameraAxis.Y, CameraAxis.X, 0.0f });	
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
	FrontSuspensions->TurnWheel(axis);
}

void APawnCar::MoveCar(float axis)
{	
	//simple move
	FVector MoveVec = GetActorRotation().RotateVector({ 0.0f, -500.0f * axis, 0.0f });	
	MeshBody->AddForce(MoveVec, NAME_None, true);
}

void APawnCar::CameraYaw(float Axis)
{
	CameraAxis.X = Axis;
}

void APawnCar::CameraPitch(float Axis)
{
	CameraAxis.Y = Axis;
}

void APawnCar::CamMoveLeft()
{
	SpringArm->AddRelativeLocation({ 0.0f, -100.0f, 0.0f });
}

void APawnCar::CamMoveReight()
{
	SpringArm->AddRelativeLocation({ 0.0f, 100.0f, 0.0f });
}