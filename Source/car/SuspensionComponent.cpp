// Fill out your copyright notice in the Description page of Project Settings.
#include "SuspensionComponent.h"

#include <string>
#include <memory>

#include "tools/debug.h"
#include "SuspensionBlockComponent.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"


// Sets default values for this component's properties
USuspensionComponent::USuspensionComponent()
	: TrackWidth(1480.0f)
	, RelaxDamperLength(1000.0f)
	, DamperMove(400.0f)
	, Stiffness(4000.0f)
	, Damper(40000.0f)
	, WheelRadius(440.0f)
	, WheelWidth(350.0f)
	, WheelMass(50.0f)
	, WheelSpringiness(0.25f)
	, KpiAngle(0.0f)
	, FrictionKof(1.0f)
	, LeftBlock()
	, RightBlock()
	, MaxTurnAngle(45.0f)
	, CurrTurnAngle(0.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
	LeftBlock = CreateDefaultSubobject<USuspensionBlockComponent>("leftBlock");
	RightBlock = CreateDefaultSubobject<USuspensionBlockComponent>("rightBlock");
	LeftBlock->SetupAttachment(this);
	RightBlock->SetupAttachment(this);
}

void USuspensionComponent::BeginPlay()
{
	Super::BeginPlay();
	//set block pos
	float HalfTrackWidth = TrackWidth / 2.0f;
	FVector LeftBlockPos = GetRelativeTransform().GetLocation();
	LeftBlockPos.X -= HalfTrackWidth;
	LeftBlock->SetRelativeLocation({ -HalfTrackWidth, 0.0f, 0.0f });

	FVector RightBlockPos = GetRelativeTransform().GetLocation();
	RightBlockPos.X -= HalfTrackWidth;
	RightBlock->SetRelativeLocation({ HalfTrackWidth, 0.0f, 0.0f });
}

void USuspensionComponent::Init(float mass, const FFuncForce& funcAddForceAtbody)
{
	FCommonSuspensionDataPtr CommonData(new FCommonSuspensionData{ mass, RelaxDamperLength, DamperMove, Stiffness,
		Damper, WheelRadius, WheelWidth, WheelMass, WheelSpringiness, KpiAngle, FrictionKof, funcAddForceAtbody });
	LeftBlock->Init(CommonData, true);
	RightBlock->Init(CommonData, false);
}

void USuspensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	tools::DebugPoint(GetWorld(), GetComponentLocation(), FColor::Green, "Suss bar");
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USuspensionComponent::TurnWheel(float Axis)
{
	if (Axis != 0.0f)
	{
		Axis *= 1.0f;
		if (MaxTurnAngle > abs(CurrTurnAngle + Axis))
		{
			CurrTurnAngle += Axis;
			LeftBlock->TurnWheel(CurrTurnAngle);
			RightBlock->TurnWheel(CurrTurnAngle);
		}
	}
}


