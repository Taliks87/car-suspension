// Fill out your copyright notice in the Description page of Project Settings.
#include "CarPawn.h"
#include "Engine.h"

#include "SuspensionComponent.h"
#include <array>

// Sets default values
ACarPawn::ACarPawn()
	: MeshBodyComponent()
	, Mass(1300.0f)
	, WeightDistribution(0.0f, -1000.0f, -300.0f)
	, FrontSuspensionsComponent()
	, RearSuspensionsComponent()
	, SpringArmComponent()
	, CameraComponent()
{
 	PrimaryActorTick.bCanEverTick = true;
	MeshBodyComponent = CreateDefaultSubobject<UStaticMeshComponent>("mesh_body");
	FrontSuspensionsComponent = CreateDefaultSubobject<USuspensionComponent>("front_suspension");
	RearSuspensionsComponent = CreateDefaultSubobject<USuspensionComponent>("rear_suspension");

	RootComponent = MeshBodyComponent;
	MeshBodyComponent->SetSimulatePhysics(true);
	FrontSuspensionsComponent->SetupAttachment(MeshBodyComponent);
	RearSuspensionsComponent->SetupAttachment(MeshBodyComponent);

	FrontSuspensionsComponent->SetRelativeLocation({ 0.0f, -1800.0f, -730.0f });
	RearSuspensionsComponent->SetRelativeLocation({ 0.0f, 1950.0f, -730.0f });

	//Init camera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("spring_arm_camera");
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("camera");
	SpringArmComponent->TargetArmLength = 4000.0f;
	SpringArmComponent->SetupAttachment(MeshBodyComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);

	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 6.0f;
	SpringArmComponent->bDrawDebugLagMarkers = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ACarPawn::BeginPlay()
{
	MeshBodyComponent->SetMassOverrideInKg(NAME_None, Mass);
	MeshBodyComponent->SetCenterOfMass(WeightDistribution);
	auto FuncAddForceAtbody = std::bind(&UStaticMeshComponent::AddForceAtLocation, MeshBodyComponent, std::placeholders::_1, std::placeholders::_2, NAME_None);
	FrontSuspensionsComponent->Init(Mass, FuncAddForceAtbody);
	RearSuspensionsComponent->Init(Mass, FuncAddForceAtbody);
	Super::BeginPlay();
}

void ACarPawn::Tick(float DeltaTime)
{
	GEngine->ClearOnScreenDebugMessages();
	Super::Tick(DeltaTime);

	SpringArmComponent->AddRelativeRotation({ CameraAxis.Y, CameraAxis.X, 0.0f });
}

void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAction("cam_move_front", IE_Pressed, this, &ACarPawn::CamMoveFront);
	InputComponent->BindAction("cam_move_back", IE_Pressed, this, &ACarPawn::CamMoveBack);
	InputComponent->BindAxis("turn_left", this, &ACarPawn::TurnWheel);
	InputComponent->BindAxis("turn_right", this, &ACarPawn::TurnWheel);
	InputComponent->BindAxis("move_front", this, &ACarPawn::MoveCar);
	InputComponent->BindAxis("move_back", this, &ACarPawn::MoveCar);
	InputComponent->BindAxis("camera_yaw", this, &ACarPawn::CameraYaw);
	InputComponent->BindAxis("camera_pitch", this, &ACarPawn::CameraPitch);
	InputComponent->BindAxis("camera_zoom", this, &ACarPawn::CameraZoom);
}

void ACarPawn::TurnWheel(float axis)
{
	FrontSuspensionsComponent->TurnWheel(axis);
}

void ACarPawn::MoveCar(float axis)
{
	//simple move
	FVector MoveVec = GetActorRotation().RotateVector({ 0.0f, -1000.0f * axis, 0.0f });
	MeshBodyComponent->AddForce(MoveVec, NAME_None, true);
}

void ACarPawn::CameraYaw(float Axis)
{
	CameraAxis.X = Axis;
}

void ACarPawn::CameraPitch(float Axis)
{
	CameraAxis.Y = Axis;
}

void ACarPawn::CameraZoom(float Axis)
{
	float& ArmLength = SpringArmComponent->TargetArmLength;
	ArmLength += Axis*-200.0f;
	if (ArmLength > 10000.0f) ArmLength = 10000.0f;
	else if (ArmLength < 3500.0f) ArmLength = 3500.0f;
}

void ACarPawn::CamMoveFront()
{
	SpringArmComponent->AddRelativeLocation({ 0.0f, -100.0f, 0.0f });
}

void ACarPawn::CamMoveBack()
{
	SpringArmComponent->AddRelativeLocation({ 0.0f, 100.0f, 0.0f });
}

