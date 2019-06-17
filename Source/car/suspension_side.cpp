#include "suspension_side.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Core/Public/Math/TransformVectorized.h"
#include "Engine.h"
#include "tools/debug.h"

// Sets default values for this component's properties
USuspensionSide::USuspensionSide()
	: mesh_wheel()
	, scene_wheelCenter()
	, scene_damperPointTop()
	, scene_damperPointBot()
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

	PrimaryComponentTick.bCanEverTick = false;	
	scene_damperPointTop->SetupAttachment(this);
	scene_damperPointBot->SetupAttachment(this);
	mesh_wheel->SetupAttachment(scene_damperPointBot);
	scene_wheelCenter->SetupAttachment(mesh_wheel);
}

void USuspensionSide::Init(tools::CommonSuspensionDataPtr& newSuspensionData, bool isLeftSide)
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
	scene_damperPointTop->SetRelativeRotation({ -90.0f, 0.0f, 0.0f }); //TODO: use kpiAngle;		
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
	const auto& endMax = start + (scene_wheelCenter->GetComponentRotation() + FRotator(-90.0f, 0.0f, 0.0f)).Vector() * (data->wheelRadius + remainingDamperLength);
	FHitResult outHit;
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this->GetAttachmentRootActor());
	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, endMax, ECC_WorldStatic, collisionParams);
	float suspensionForce;
	FVector frictionForceVec = FVector::ZeroVector;
	if (isHit && outHit.bBlockingHit && GEngine)
	{
		//calc motion length
		FVector hitPos = outHit.ImpactPoint;
		tools::DubugPoint(GetWorld(), hitPos, FColor::Red, "hitPos");
		float motionLength = outHit.Distance - data->wheelRadius;
		scene_damperPointBot->AddRelativeLocation(FRotator(-90.f, 0.f, 0.f).Vector() * (motionLength)); //TODO: use kpiAngle;							
		oldDamperLength = currDamperLength;		
		currDamperLength += motionLength;
		damperForce = data->damper * motionLength;
		springForce = data->stiffness * (currDamperLength - data->relaxDamperLength);
		suspensionForce = springForce + damperForce;

		if (suspensionForce < 0) {// decompression
			//friction
			auto tr = mesh_wheel->GetComponentTransform();
			FVector velocityAyHitPoint = mesh_wheel->GetPhysicsLinearVelocityAtPoint(hitPos);
			velocityAyHitPoint = tr.GetRotation().UnrotateVector(velocityAyHitPoint);
			float frictioForce = velocityAyHitPoint.X * data->frictionKof * -suspensionForce;
			float maxFrictioForce = velocityAyHitPoint.X * data->commonMass;
			if (abs(frictioForce) > abs(maxFrictioForce)) frictioForce = maxFrictioForce;
			frictionForceVec = { -frictioForce, 0.0f, 0.0f };
			frictionForceVec = tr.GetRotation().RotateVector(frictionForceVec);
			data->addForceAtBody(frictionForceVec, start, NAME_None);
			
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "frictionForceVec " + frictionForceVec.ToString());
			DrawDebugLine(GetWorld(), hitPos, hitPos +
				frictionForceVec, FColor::Blue, false);
		}				
		DrawDebugLine(GetWorld(), scene_damperPointTop->GetComponentLocation(), scene_damperPointTop->GetComponentLocation() +
			mesh_wheel->GetPhysicsLinearVelocityAtPoint(hitPos), FColor::White, false);	//wheel velocity	
	}
	else {
		const auto& leftDumperTr = scene_damperPointTop->GetRelativeTransform();
		scene_damperPointBot->SetRelativeLocation(leftDumperTr.GetLocation() + leftDumperTr.GetRotation().Vector() * (data->relaxDamperLength + data->damperMove));

		oldDamperLength = currDamperLength;
		currDamperLength = data->relaxDamperLength + data->damperMove;
		suspensionForce = 0;		
	}

	//mesh_wheel->GetComponentVelocity();
	mesh_wheel->GetPhysicsLinearVelocity();
	FVector suspensionForceVec = scene_damperPointTop->GetForwardVector() * (suspensionForce);
	data->addForceAtBody(suspensionForceVec, scene_damperPointTop->GetComponentLocation(), NAME_None);	
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "suspensionForceVec " + suspensionForceVec.ToString());
	tools::DubugPoint(GetWorld(), scene_damperPointBot->GetComponentLocation(), FColor::Green, "damper bot");
	tools::DubugPoint(GetWorld(), scene_damperPointTop->GetComponentLocation(), FColor::Green, "damper top");	
	DrawDebugLine(GetWorld(), start, endMax, FColor::Purple, false);
	DrawDebugLine(GetWorld(), scene_damperPointTop->GetComponentLocation(), scene_damperPointTop->GetComponentLocation() +
		suspensionForceVec / 100.0f, FColor::Blue, false);

	tools::DubugPointOnScreen(GetWorld(), scene_wheelCenter->GetComponentLocation(), FColor::Green, "wheel center");
	/*GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "damper length " + FString::SanitizeFloat(currDamperLength));
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "spring force " + FString::SanitizeFloat(springForce));
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "damper force " + FString::SanitizeFloat(damperForce));*/
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::White, "velocity " + mesh_wheel->GetPhysicsLinearVelocity().ToString());
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Purple, this->GetName());
}

