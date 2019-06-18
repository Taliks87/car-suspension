// Fill out your copyright notice in the Description page of Project Settings.
#include "suspension.h"

#include <string>
#include <memory>

#include "tools/debug.h"
#include "suspension_side.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"

// Sets default values for this component's properties
USuspension::USuspension()
	: trackWidth(1480.0f)
	, relaxDamperLength(1000.0f)
	, damperMove(400.0f)
	, stiffness(4000.0f)
	, damper(40000.0f)
	, wheelRadius(440.0f)
	, wheelWidth(350.0f)
	, wheelMass(50.0f)
	, kpiAngle(0.0f)
	, frictionKof(1.0f)
	, leftBlock()
	, rightBlock()
	, maxTurnAngle(45.0f)
	, currTurnAngle(0.0f)
{	
	PrimaryComponentTick.bCanEverTick = true;
	leftBlock = CreateDefaultSubobject<USuspensionSide>("leftBlock");
	rightBlock = CreateDefaultSubobject<USuspensionSide>("rightBlock");	
	leftBlock->SetupAttachment(this);
	rightBlock->SetupAttachment(this);
}

void USuspension::BeginPlay()
{	
	Super::BeginPlay();				
	//set block pos
	float halfTrackWidth = trackWidth / 2.0f;
	FVector leftBlockPos = GetRelativeTransform().GetLocation();
	leftBlockPos.X -= halfTrackWidth;
	leftBlock->SetRelativeLocation({ -halfTrackWidth, 0.0f, 0.0f });

	FVector rightBlockPos = GetRelativeTransform().GetLocation();	
	rightBlockPos.X -= halfTrackWidth;
	rightBlock->SetRelativeLocation({ halfTrackWidth, 0.0f, 0.0f });	
}

void USuspension::Init(float mass, const tools::FuncForce& funcAddForceAtbody)
{
	tools::CommonSuspensionDataPtr commonData = std::make_shared<tools::CommonSuspensionData>(mass, relaxDamperLength, damperMove, stiffness,
		damper, wheelRadius, wheelWidth, wheelMass, kpiAngle, frictionKof, funcAddForceAtbody);
	leftBlock->Init(commonData, true);
	rightBlock->Init(commonData, false);
}

void USuspension::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{	
	tools::DubugPoint(GetWorld(), GetComponentLocation(), FColor::Green, "Suss bar");
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//leftBlock->TickComponent(DeltaTime, TickType, ThisTickFunction);
	//rightBlock->TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void USuspension::turnWheel(float axis)
{
	if (axis != 0.0f)
	{
		axis *= 1.0f;
		if (maxTurnAngle > abs(currTurnAngle + axis))
		{
			currTurnAngle += axis;
			leftBlock->turnWheel(currTurnAngle);
			rightBlock->turnWheel(currTurnAngle);
		}
	}
}


