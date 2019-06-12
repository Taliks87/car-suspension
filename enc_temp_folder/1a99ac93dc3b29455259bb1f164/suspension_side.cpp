#include "suspension_side.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"
#include "tools/debug.h"

// Sets default values for this component's properties
USuspensionSide::USuspensionSide()
	: mesh_wheel(CreateDefaultSubobject<UStaticMeshComponent>("mesh_wheel"))
	, scene_wheelCenter(CreateDefaultSubobject<USceneComponent>("wheelCenter"))
	, scene_damperPointTop(CreateDefaultSubobject<USceneComponent>("damperPointTop"))
	, scene_damperPointBot(CreateDefaultSubobject<USceneComponent>("damperPointBot"))
	, scene_botPoint(CreateDefaultSubobject<USceneComponent>("botPoint"))
	, isLeft(false)
	, springForce(0.0f)
	, reyLength(0.0f)
	, currDamperLength(0.0f)
	, oldDamperLength(0.0f)	
{	
	PrimaryComponentTick.bCanEverTick = true;	
}

void USuspensionSide::Init(tools::SuspensionDataPtr& newSuspensionData, bool isLeftSide)
{
	isLeft = isLeftSide;
	data = newSuspensionData;
	currDamperLength = data->relaxDamperLength;
}

void USuspensionSide::BeginPlay()
{
	scene_damperPointTop->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	scene_damperPointBot->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	mesh_wheel->AttachToComponent(scene_damperPointBot, FAttachmentTransformRules::SnapToTargetIncludingScale);
	scene_wheelCenter->AttachToComponent(mesh_wheel, FAttachmentTransformRules::SnapToTargetIncludingScale);
	scene_botPoint->AttachToComponent(scene_wheelCenter, FAttachmentTransformRules::SnapToTargetIncludingScale);

	Super::BeginPlay();	

	//set damper pos
	scene_damperPointTop->SetRelativeLocation({ 0.0f, 0.0f, data->relaxDamperLength });
	scene_damperPointTop->SetRelativeRotation({ -90.0f, 0.0f, 0.0f }); //SetRelativeRotation(kpiAngle);		
	//set wheel pos
	const auto& leftDumperTr = scene_damperPointTop->GetRelativeTransform();
	FVector posWheel = leftDumperTr.GetLocation() + leftDumperTr.GetRotation().Vector() * data->relaxDamperLength;
	//leftBlock->mesh_wheel->SetRelativeLocation(posWheelFL);		
	scene_damperPointBot->SetRelativeLocation(posWheel);
	FVector posWheelCenter;
	if (isLeft) {
		posWheelCenter = posWheel + (mesh_wheel->GetComponentRotation().Vector() * (-data->wheelWidth / 2.0f));
	} else {
		posWheelCenter = posWheel - (mesh_wheel->GetComponentRotation().Vector() * (-data->wheelWidth / 2.0f));
	}
	
	scene_wheelCenter->SetRelativeLocation(posWheelCenter);
	//set bot point
	scene_botPoint->SetRelativeLocation(posWheelCenter + FVector(0.0f, 0.0f, -data->wheelRadius));
}

void USuspensionSide::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);	
	RefreshBlock(DeltaTime);
}

void USuspensionSide::RefreshBlock(float DeltaTime)
{
	float remainingDamperLength = data->relaxDamperLength + data->springMove - currDamperLength;

	const auto& start = scene_wheelCenter->GetComponentLocation();
	//const auto& end = start + (scene_wheelCenter->GetComponentRotation() + FRotator(-90.0f, 0.0f, 0.0f)).Vector() * (data->wheelRadius);
	const auto& endMax = start + (scene_wheelCenter->GetComponentRotation() + FRotator(-90.0f, 0.0f, 0.0f)).Vector() * (data->wheelRadius + remainingDamperLength);
	FHitResult outHit;
	FCollisionQueryParams collisionParams;
	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, endMax, ECC_WorldDynamic, collisionParams);
	if (isHit && outHit.bBlockingHit && /*(outHit.GetComponent() != this->GetAttachParent()) &&*/ GEngine)
	{
		//calc motion length
		FVector hitPos = outHit.ImpactPoint;
		tools::DubugPoint(GetWorld(), hitPos, FColor::Red, "hitPos");
		float motionLength = outHit.Distance - data->wheelRadius;
		if (motionLength < 0)
		{
			scene_damperPointBot->AddRelativeLocation(FRotator(-90.f, 0.f, 0.f).Vector() * (motionLength)); //TODO: if angle isn't 90								
		}
		else {
			scene_damperPointBot->AddRelativeLocation(FRotator(-90.f, 0.f, 0.f).Vector() * (motionLength));//TODO: if angle isn't 90								
		}
		oldDamperLength = currDamperLength;
		currDamperLength += motionLength;
		springForce = data->stiffness * 100.f * (currDamperLength - data->relaxDamperLength);

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "moveLength " + FString::SanitizeFloat(motionLength));
	}
	else {
		const auto& leftDumperTr = scene_damperPointTop->GetRelativeTransform();
		scene_damperPointBot->SetRelativeLocation(leftDumperTr.GetLocation() + leftDumperTr.GetRotation().Vector() * (data->relaxDamperLength + data->springMove));

		oldDamperLength = currDamperLength;
		currDamperLength = data->relaxDamperLength + data->springMove;
		springForce = 0;
	}

	data->addForceAtBody(scene_damperPointTop->GetForwardVector() * (springForce), scene_damperPointTop->GetComponentLocation(), NAME_None);

	//tools::DubugPoint(GetWorld(), suspSide->GetComponentLocation(), FColor::Green, "block");
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "currSpringLength " + FString::SanitizeFloat(currDamperLength));
	tools::DubugPoint(GetWorld(), scene_wheelCenter->GetComponentLocation(), FColor::Green, "wheel center");
	tools::DubugPoint(GetWorld(), scene_damperPointBot->GetComponentLocation(), FColor::Green, "damper bot");
	tools::DubugPoint(GetWorld(), mesh_wheel->GetComponentLocation(), FColor::Green, "wheel");
	tools::DubugPoint(GetWorld(), scene_botPoint->GetComponentLocation(), FColor::Green, "bot point");
	tools::DubugPoint(GetWorld(), scene_damperPointTop->GetComponentLocation(), FColor::Green, "damper top");

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, FString::SanitizeFloat(springForce));
	DrawDebugLine(GetWorld(), start, endMax, FColor::Purple, false);
	DrawDebugLine(GetWorld(), scene_damperPointTop->GetComponentLocation(), scene_damperPointTop->GetComponentLocation() +
		scene_damperPointTop->GetForwardVector() * (springForce / 100.0f), FColor::Purple, false);
}

