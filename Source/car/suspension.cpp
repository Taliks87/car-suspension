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
	: trackWidth(1520.0f)
	, relaxDamperLength(810.0f)
	, springMove(200.0f)
	, stiffness(4.5f)
	, damper(100.0f)
	, wheelRadius(440.0f)
	, wheelWidth(350.0f)
	, kpiAngle(0.0f)	
	, leftBlock(CreateDefaultSubobject<USuspensionSide>("leftBlock"))
	, rightBlock(CreateDefaultSubobject<USuspensionSide>("rightPart"))	
{	
	PrimaryComponentTick.bCanEverTick = true;			
}

void USuspension::BeginPlay()
{
	leftBlock->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);	
	rightBlock->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
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

void USuspension::Init(const tools::FuncForce& funcAddForceAtbody)
{
	tools::SuspensionDataPtr p = std::make_shared<tools::SuspensionData>(relaxDamperLength, springMove, stiffness,
		damper, wheelRadius, wheelWidth, kpiAngle, funcAddForceAtbody);
	leftBlock->Init(p, true);	
	rightBlock->Init(p, false);
}

void USuspension::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	tools::DubugPoint(GetWorld(), GetComponentLocation(), FColor::Green, "Suss bar");
}




