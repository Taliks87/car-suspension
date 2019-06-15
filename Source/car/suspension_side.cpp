#include "suspension_side.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"
#include "tools/debug.h"
//#include "CollisionQueryParams.h"

// Sets default values for this component's properties
USuspensionSide::USuspensionSide()
	: mesh_wheel()
	, scene_wheelCenter()
	, scene_damperPointTop()
	, scene_damperPointBot()
	//, scene_botPoint()
	, isLeft(false)
	, springForce(0.0f)
	, damperForce(0.0f)
	, reyLength(0.0f)
	, currDamperLength(0.0f)
	, oldDamperLength(0.0f)	
	, maxDamperLength(0.0f)
	, data()
{	
	mesh_wheel = CreateDefaultSubobject<UStaticMeshComponent>("mesh_wheel");
	scene_wheelCenter = CreateDefaultSubobject<USceneComponent>("wheelCenter");
	scene_damperPointTop = CreateDefaultSubobject<USceneComponent>("damperPointTop");
	scene_damperPointBot = CreateDefaultSubobject<USceneComponent>("damperPointBot");
	//scene_botPoint = CreateDefaultSubobject<USceneComponent>("botPoint");

	PrimaryComponentTick.bCanEverTick = false;	
	scene_damperPointTop->SetupAttachment(this);
	scene_damperPointBot->SetupAttachment(this);
	mesh_wheel->SetupAttachment(scene_damperPointBot);
	scene_wheelCenter->SetupAttachment(mesh_wheel);
	//scene_botPoint->SetupAttachment(scene_damperPointBot);
}

void USuspensionSide::Init(tools::SuspensionDataPtr& newSuspensionData, bool isLeftSide)
{
	isLeft = isLeftSide;
	data = newSuspensionData;
	currDamperLength = data->relaxDamperLength;
	maxDamperLength = data->relaxDamperLength + data->damperMove;
}

void USuspensionSide::BeginPlay()
{
	Super::BeginPlay();	

	//set damper pos
	scene_damperPointTop->SetRelativeLocation({ 0.0f, 0.0f, data->relaxDamperLength });
	scene_damperPointTop->SetRelativeRotation({ -90.0f, 0.0f, 0.0f }); //SetRelativeRotation(kpiAngle);		
	//set wheel pos
	const auto& leftDumperTr = scene_damperPointTop->GetRelativeTransform();
	FVector posWheel = leftDumperTr.GetLocation() + leftDumperTr.GetRotation().Vector() * data->relaxDamperLength;
	scene_damperPointBot->SetRelativeLocation(posWheel);
	FVector posWheelCenter;
	if (isLeft) {
		posWheelCenter = posWheel + (mesh_wheel->GetComponentRotation().Vector() * (-data->wheelWidth / 2.0f));
	} else {
		posWheelCenter = posWheel - (mesh_wheel->GetComponentRotation().Vector() * (-data->wheelWidth / 2.0f));
	}
	
	scene_wheelCenter->SetRelativeLocation(posWheelCenter);
	//set bot point
	//scene_botPoint->SetRelativeLocation(posWheelCenter + FVector(0.0f, 0.0f, - data->wheelRadius - data->damperMove));
}

void USuspensionSide::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);	
	RefreshBlock(DeltaTime);
}

void USuspensionSide::turnWheel(float angle)
{
	scene_damperPointBot->SetRelativeRotation({0.0f, angle, 0.0f});
}

void USuspensionSide::RefreshBlock(float DeltaTime)
{
	float remainingDamperLength = maxDamperLength - currDamperLength;

	const auto& start = scene_wheelCenter->GetComponentLocation();
	//const auto& end = start + (scene_wheelCenter->GetComponentRotation() + FRotator(-90.0f, 0.0f, 0.0f)).Vector() * (data->wheelRadius);
	const auto& endMax = start + (scene_wheelCenter->GetComponentRotation() + FRotator(-90.0f, 0.0f, 0.0f)).Vector() * (data->wheelRadius + remainingDamperLength);
	FHitResult outHit;
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this->GetAttachmentRootActor());
	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, endMax, ECC_WorldStatic, collisionParams);
	if (isHit && outHit.bBlockingHit && /*(outHit.GetComponent() != this->GetAttachParent()) &&*/ GEngine)
	{
		//calc motion length
		FVector hitPos = outHit.ImpactPoint;
		tools::DubugPoint(GetWorld(), hitPos, FColor::Red, "hitPos");
		float motionLength = outHit.Distance - data->wheelRadius;
		if (motionLength < 0) {// decompression
			scene_damperPointBot->AddRelativeLocation(FRotator(-90.f, 0.f, 0.f).Vector() * (motionLength)); //TODO: if angle isn't 90								
		} else {// compression
			scene_damperPointBot->AddRelativeLocation(FRotator(-90.f, 0.f, 0.f).Vector() * (motionLength));//TODO: if angle isn't 90								
		}
		oldDamperLength = currDamperLength;		
		currDamperLength += motionLength;
		damperForce = data->damper * motionLength;
		springForce = data->stiffness * (currDamperLength - data->relaxDamperLength);		

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "moveLength " + FString::SanitizeFloat(motionLength));
	}
	else {
		const auto& leftDumperTr = scene_damperPointTop->GetRelativeTransform();
		scene_damperPointBot->SetRelativeLocation(leftDumperTr.GetLocation() + leftDumperTr.GetRotation().Vector() * (data->relaxDamperLength + data->damperMove));

		oldDamperLength = currDamperLength;
		currDamperLength = data->relaxDamperLength + data->damperMove;
		springForce = 0;
		damperForce = 0;
	}

	data->addForceAtBody(scene_damperPointTop->GetForwardVector() * (springForce + damperForce), scene_damperPointTop->GetComponentLocation(), NAME_None);
	
	//tools::DubugPoint(GetWorld(), GetComponentLocation(), FColor::Green, "block");			
	tools::DubugPoint(GetWorld(), scene_damperPointBot->GetComponentLocation(), FColor::Green, "damper bot");
	//tools::DubugPoint(GetWorld(), mesh_wheel->GetComponentLocation(), FColor::Green, "wheel");
	//tools::DubugPoint(GetWorld(), scene_botPoint->GetComponentLocation(), FColor::Green, "bot point");
	tools::DubugPoint(GetWorld(), scene_damperPointTop->GetComponentLocation(), FColor::Green, "damper top");	
	DrawDebugLine(GetWorld(), start, endMax, FColor::Purple, false);
	DrawDebugLine(GetWorld(), scene_damperPointTop->GetComponentLocation(), scene_damperPointTop->GetComponentLocation() +
		scene_damperPointTop->GetForwardVector() * ((springForce + damperForce) / 100.0f), FColor::Blue, false);
	tools::DubugPointOnScreen(GetWorld(), scene_wheelCenter->GetComponentLocation(), FColor::Green, "wheel center");
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "damper length " + FString::SanitizeFloat(currDamperLength));
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "spring force " + FString::SanitizeFloat(springForce));
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "damper force " + FString::SanitizeFloat(damperForce));
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::White, this->GetName());
}

