// Fill out your copyright notice in the Description page of Project Settings.
#include "suspension.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"
#include "tools/debug.h"
#include <string>

#include "suspension_side.h"

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
	//, right(CreateDefaultSubobject<USuspensionSide>("rightPart"))	
{	
	PrimaryComponentTick.bCanEverTick = true;			
}

void USuspension::BeginPlay()
{
	leftBlock->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	leftBlock->mesh_wheel->AttachToComponent(leftBlock, FAttachmentTransformRules::SnapToTargetIncludingScale);
	leftBlock->scene_damperPointBot->AttachToComponent(leftBlock, FAttachmentTransformRules::SnapToTargetIncludingScale);
	leftBlock->scene_damperPointTop->AttachToComponent(leftBlock, FAttachmentTransformRules::SnapToTargetIncludingScale);
	leftBlock->scene_damperPointBot->AttachToComponent(leftBlock, FAttachmentTransformRules::SnapToTargetIncludingScale);
	leftBlock->scene_botPoint->AttachToComponent(leftBlock, FAttachmentTransformRules::SnapToTargetIncludingScale);

	Super::BeginPlay();				
	//set block pos
	FVector leftBlockPos = GetRelativeTransform().GetLocation();
	float halfTrackWidth = trackWidth / 2.0f;
	leftBlockPos.X -= halfTrackWidth;
	leftBlock->SetRelativeLocation({ -halfTrackWidth, 0.0f, 0.0f });
	//set damper pos
	leftBlock->scene_damperPointTop->SetRelativeLocation({ 0.0f, 0.0f, relaxDamperLength });
	leftBlock->scene_damperPointTop->SetRelativeRotation({ -90.0f, 0.0f, 0.0f }); //SetRelativeRotation(kpiAngle);		
	//set wheel pos
	const auto& leftDumperTr = leftBlock->scene_damperPointTop->GetRelativeTransform();
	FVector posWheelFL = leftDumperTr.GetLocation() + leftDumperTr.GetRotation().Vector() * relaxDamperLength;		
	leftBlock->mesh_wheel->SetRelativeLocation(posWheelFL);		
	leftBlock->scene_wheelCenter->SetRelativeLocation(posWheelFL);
	FVector posWheelCenter = posWheelFL + (leftBlock->mesh_wheel->GetComponentRotation().Vector() * (-wheelWidth/2.0f));
	leftBlock->scene_damperPointBot->SetRelativeLocation(posWheelCenter);
	//set bot point
	leftBlock->scene_botPoint->SetRelativeLocation(posWheelCenter + FVector(0.0f, 0.0f, -(wheelRadius + springMove)));
}

void USuspension::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	tools::DubugPoint(GetWorld(), GetComponentLocation(), FColor::Green, "Suss bar");
	//DubugPoint(GetWorld(), GetComponentLocation(), FColor::Green, "Suss bar");
	//DubugPoint(GetWorld(), left->GetComponentLocation(), FColor::Green, "LPartSuss");	

	refreshBlock(DeltaTime, leftBlock);
}

void USuspension::refreshBlock(float DeltaTime, USuspensionSide* suspSide)
{
	
	const auto& start = suspSide->scene_damperPointBot->GetComponentLocation();	
	const auto& end = start + (suspSide->scene_damperPointBot->GetComponentRotation() + FRotator(-90.0f, 0.0f, 0.0f)).Vector() * (wheelRadius + springMove);
	FHitResult outHit;
	FCollisionQueryParams collisionParams;	
	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_WorldStatic, collisionParams);
	if (isHit && outHit.bBlockingHit && GEngine)
	{
		FVector hitPos = outHit.ImpactPoint;
		tools::DubugPoint(GetWorld(), hitPos, FColor::Red, "hitPos");
		float moveLength;
		const FVector& botPoint = suspSide->scene_botPoint->GetComponentLocation();
		moveLength = FVector::Distance(botPoint, hitPos);//TODO
		suspSide->oldDamperLength = suspSide->currDamperLength;
		suspSide->currDamperLength = relaxDamperLength - moveLength;
		suspSide->springForce = stiffness * (relaxDamperLength - suspSide->currDamperLength) / 10.f;		

		leftBlock->mesh_wheel->AddRelativeLocation((suspSide->scene_wheelCenter->GetComponentRotation() + FRotator(-90.f, 0.f, 0.f)).Vector() * (-moveLength));

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "moveLength " + FString::SanitizeFloat(moveLength));		
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "currSpringLength " + FString::SanitizeFloat(suspSide->currDamperLength));
	}
	else {
		//Forces	
		suspSide->currDamperLength = relaxDamperLength;		
		suspSide->springForce = 0;
	}		

	//tools::DubugPoint(GetWorld(), suspSide->GetComponentLocation(), FColor::Green, "block");
	tools::DubugPoint(GetWorld(), suspSide->scene_wheelCenter->GetComponentLocation(), FColor::Green, "wheel center");
	tools::DubugPoint(GetWorld(), suspSide->scene_damperPointBot->GetComponentLocation(), FColor::Green, "damper bop");
	tools::DubugPoint(GetWorld(), suspSide->mesh_wheel->GetComponentLocation(), FColor::Green, "wheel");
	tools::DubugPoint(GetWorld(), suspSide->scene_botPoint->GetComponentLocation(), FColor::Green, "bot point");
	tools::DubugPoint(GetWorld(), suspSide->scene_damperPointTop->GetComponentLocation(), FColor::Green, "damper top");
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, FString::SanitizeFloat(suspSide->springForce));
	DrawDebugLine(GetWorld(), start, end, FColor::Purple, false, 0.5f);
}




