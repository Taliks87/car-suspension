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
	, addForceAtbody()
{	
	PrimaryComponentTick.bCanEverTick = true;			
}

void USuspension::BeginPlay()
{
	leftBlock->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);	
	leftBlock->scene_damperPointTop->AttachToComponent(leftBlock, FAttachmentTransformRules::SnapToTargetIncludingScale);
	leftBlock->scene_damperPointBot->AttachToComponent(leftBlock, FAttachmentTransformRules::SnapToTargetIncludingScale);	
	leftBlock->mesh_wheel->AttachToComponent(leftBlock->scene_damperPointBot, FAttachmentTransformRules::SnapToTargetIncludingScale);
	leftBlock->scene_wheelCenter->AttachToComponent(leftBlock->mesh_wheel, FAttachmentTransformRules::SnapToTargetIncludingScale);
	leftBlock->scene_botPoint->AttachToComponent(leftBlock->scene_wheelCenter, FAttachmentTransformRules::SnapToTargetIncludingScale);
	leftBlock->currDamperLength = relaxDamperLength;

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
	//leftBlock->mesh_wheel->SetRelativeLocation(posWheelFL);		
	leftBlock->scene_damperPointBot->SetRelativeLocation(posWheelFL);
	FVector posWheelCenter = posWheelFL + (leftBlock->mesh_wheel->GetComponentRotation().Vector() * (-wheelWidth/2.0f));
	leftBlock->scene_wheelCenter->SetRelativeLocation(posWheelCenter);
	//set bot point
	leftBlock->scene_botPoint->SetRelativeLocation(posWheelCenter + FVector(0.0f, 0.0f, -wheelRadius));
}

void USuspension::init(const FuncForce& funcAddForceAtbody)
{
	addForceAtbody = funcAddForceAtbody;
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
	const auto& start = suspSide->scene_wheelCenter->GetComponentLocation();
	const auto& end = start + (suspSide->scene_wheelCenter->GetComponentRotation() + FRotator(-90.0f, 0.0f, 0.0f)).Vector() * (wheelRadius);
	const auto& endMax = start + (suspSide->scene_wheelCenter->GetComponentRotation() + FRotator(-90.0f, 0.0f, 0.0f)).Vector() * (wheelRadius + springMove);
	FHitResult outHit;
	FCollisionQueryParams collisionParams;	
	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, endMax, ECC_WorldDynamic, collisionParams);
	if (isHit && outHit.bBlockingHit && /*(outHit.GetComponent() != this->GetAttachParent()) &&*/ GEngine)
	{
		//calc motion length
		FVector hitPos = outHit.ImpactPoint;
		tools::DubugPoint(GetWorld(), hitPos, FColor::Red, "hitPos");
		float motionLength = outHit.Distance - wheelRadius;
		if(motionLength < 0)
		{			
			suspSide->scene_damperPointBot->AddRelativeLocation(FRotator(-90.f, 0.f, 0.f).Vector() * (motionLength)); //TODO: if angle isn't 90								
		} else {
			suspSide->scene_damperPointBot->AddRelativeLocation(FRotator(-90.f, 0.f, 0.f).Vector() * (motionLength));//TODO: if angle isn't 90								
		}
		suspSide->oldDamperLength = suspSide->currDamperLength;
		suspSide->currDamperLength += motionLength;
		suspSide->springForce = stiffness * 1000.f * (suspSide->currDamperLength - relaxDamperLength) / 10.f;

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "moveLength " + FString::SanitizeFloat(motionLength));
	} else {
		const auto& leftDumperTr = leftBlock->scene_damperPointTop->GetRelativeTransform();
		suspSide->scene_damperPointBot->SetRelativeLocation(leftDumperTr.GetLocation() + leftDumperTr.GetRotation().Vector() * (relaxDamperLength + springMove));

		suspSide->oldDamperLength = suspSide->currDamperLength;
		suspSide->currDamperLength = relaxDamperLength + springMove;
		suspSide->springForce = 0;
	}			
	
	
	addForceAtbody(suspSide->scene_damperPointTop->GetForwardVector() * (suspSide->springForce), suspSide->scene_damperPointTop->GetComponentLocation(), NAME_None);	

	//tools::DubugPoint(GetWorld(), suspSide->GetComponentLocation(), FColor::Green, "block");
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "currSpringLength " + FString::SanitizeFloat(suspSide->currDamperLength));
	tools::DubugPoint(GetWorld(), suspSide->scene_wheelCenter->GetComponentLocation(), FColor::Green, "wheel center");
	tools::DubugPoint(GetWorld(), suspSide->scene_damperPointBot->GetComponentLocation(), FColor::Green, "damper bot");
	tools::DubugPoint(GetWorld(), suspSide->mesh_wheel->GetComponentLocation(), FColor::Green, "wheel");
	tools::DubugPoint(GetWorld(), suspSide->scene_botPoint->GetComponentLocation(), FColor::Green, "bot point");
	tools::DubugPoint(GetWorld(), suspSide->scene_damperPointTop->GetComponentLocation(), FColor::Green, "damper tot");
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, FString::SanitizeFloat(suspSide->springForce));
	DrawDebugLine(GetWorld(), start, end, FColor::Purple, false);
	DrawDebugLine(GetWorld(), suspSide->scene_damperPointTop->GetComponentLocation(), suspSide->scene_damperPointTop->GetComponentLocation() + 
		suspSide->scene_damperPointTop->GetForwardVector() * (suspSide->springForce/100.0f), FColor::Purple, false);
}




