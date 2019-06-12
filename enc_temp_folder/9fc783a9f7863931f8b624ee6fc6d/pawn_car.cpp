// Fill out your copyright notice in the Description page of Project Settings.
#include "pawn_car.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"

#include "suspension.h"

// Sets default values
APawnCar::APawnCar()
	: mesh_body(CreateDefaultSubobject<UStaticMeshComponent>("mesh_body"))
	, mass(100.0f)
	, weightDistribution(0.7f)
	, suspensions(TArray<USuspension*>({ CreateDefaultSubobject<USuspension>("front_suspension"), CreateDefaultSubobject<USuspension>("rear_suspension") }))
{
 	PrimaryActorTick.bCanEverTick = true;
	suspensions[0]->SetRelativeLocation({ 0.0f, -1800.0f, -730.0f });
	suspensions[1]->SetRelativeLocation({ 0.0f, 1950.0f, -730.0f });
	for (auto suspension : suspensions)
	{
		suspension->SetupAttachment(mesh_body);		
		auto f = std::bind(&UStaticMeshComponent::AddForceAtLocation, mesh_body, std::placeholders::_1, std::placeholders::_2, NAME_None);
		suspension->Init(f);
    }		
}

void APawnCar::BeginPlay()
{	
	//mesh_body->SetCenterOfMass()
	//mesh_body->SetMassScale();
	Super::BeginPlay();		
}

void APawnCar::Tick(float DeltaTime)
{		
	GEngine->ClearOnScreenDebugMessages();
	Super::Tick(DeltaTime);		
	//mesh_body->AddForceAtLocation({ 1000000.0f, 1000000.0f, 0.0f }, { 100.0f, 1000.0f, 0.0f });
}

// Called to bind functionality to input
void APawnCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);	
}