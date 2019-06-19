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
	, suspensionForce(0.0f)
	, compressionVelocity(0.0f)
	, wheelSpinVelocity(0.0f)
	, reyLength(0.0f)
	, currDamperLength(0.0f)	
	, maxDamperLength(0.0f)	
	, minDamperLength(0.0f)
	, data()
	, collisionParams()
{	
	mesh_wheel = CreateDefaultSubobject<UStaticMeshComponent>("mesh_wheel");
	scene_wheelCenter = CreateDefaultSubobject<USceneComponent>("wheelCenter");
	scene_damperPointTop = CreateDefaultSubobject<USceneComponent>("damperPointTop");
	scene_damperPointBot = CreateDefaultSubobject<USceneComponent>("damperPointBot");

	PrimaryComponentTick.bCanEverTick = true;	
	scene_damperPointTop->SetupAttachment(this);
	scene_damperPointBot->SetupAttachment(this);
	mesh_wheel->SetupAttachment(scene_damperPointBot);	
	scene_wheelCenter->SetupAttachment(scene_damperPointBot);
	collisionParams.AddIgnoredActor(this->GetAttachmentRootActor());
}

void USuspensionSide::Init(tools::CommonSuspensionDataPtr& newSuspensionData, bool isLeftSide)
{
	isLeft = isLeftSide;
	data = newSuspensionData;
	currDamperLength = data->relaxDamperLength;
	maxDamperLength = data->relaxDamperLength + data->damperMove;
	minDamperLength = data->relaxDamperLength - data->damperMove;
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
		posWheelCenter.X = -data->wheelWidth / 2.0f;
	} else {
		posWheelCenter.X = data->wheelWidth / 2.0f;
	}
	
	scene_wheelCenter->SetRelativeLocation(posWheelCenter);	
}

void USuspensionSide::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{			
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);	
	UpdateSuspension(DeltaTime);
}

void USuspensionSide::TurnWheel(float angle)
{
	scene_damperPointBot->SetRelativeRotation({0.0f, angle, 0.0f});
}

void USuspensionSide::UpdateSuspension(float deltaTime)
{			
	//rays cast
	const auto& wheelCenterLoc = scene_wheelCenter->GetComponentLocation();
	const auto& wheelCenterRot = scene_wheelCenter->GetComponentRotation();
	bool isAnyHit = false;
	float hitDistance = TNumericLimits< float >::Max();
	FVector hitPos;

	const auto& vecToWheelTop = wheelCenterRot.RotateVector(FVector::UpVector);
	const auto& topPoint = wheelCenterLoc + vecToWheelTop * (data->wheelRadius);
	{
		FHitResult outHit;
		for (float angle = -75.0f; angle <= 75.0f; angle += 15.0f)
		{
			FRotator rotator{ 0.0f, 0.0f, angle };
						
			const auto& vecToStart = (wheelCenterRot + rotator).RotateVector(FVector::DownVector);
			const auto& startPoint = topPoint + vecToStart * (data->wheelRadius);
			const auto& endPoint = startPoint + vecToWheelTop * -(data->wheelRadius + data->damperMove);

			bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, startPoint, endPoint, ECC_WorldStatic, collisionParams);
			if (isHit && outHit.bBlockingHit && GEngine)
			{
				if (outHit.Distance < hitDistance)
				{
					hitDistance = outHit.Distance;
					hitPos = outHit.ImpactPoint;
				}
				isAnyHit = true;
			}

			DrawDebugLine(GetWorld(), startPoint, endPoint, FColor::Purple, false);// Debug ray cast
		}
	}	

	//calc damper motion length					
	float maxMotionLength = maxDamperLength - currDamperLength;
	float minMotionLength = minDamperLength - currDamperLength;
	if (isAnyHit)
	{
		maxMotionLength = FMath::Max(hitDistance - data->wheelRadius, minMotionLength);					
		tools::DubugPoint(GetWorld(), hitPos, FColor::Red, "hitPos");
	}	
	bool isWeelFree = (maxMotionLength > 0.0f || !isAnyHit || hitDistance > data->wheelRadius);

	float motionLength;
	if (isWeelFree)
	{
		motionLength = ((suspensionForce / data->wheelMass) * deltaTime * deltaTime / 2 + compressionVelocity * deltaTime);
		if (motionLength > 0.0f) {// decompression
			if (motionLength >= maxMotionLength) {
				motionLength = maxMotionLength;
				isWeelFree = false;
			}
		} else {//compression
			if (motionLength < minMotionLength) motionLength = minMotionLength;				
		}			
	}
	else {
		motionLength = maxMotionLength;
	}
	compressionVelocity = motionLength / deltaTime;

	//calc suspension force
	scene_damperPointBot->AddRelativeLocation(FRotator(-90.f, 0.f, 0.f).Vector() * (motionLength)); //TODO: use kpiAngle;		
	currDamperLength += motionLength;				
	damperForce = data->damper * -motionLength;
	springForce = data->stiffness * (data->relaxDamperLength - currDamperLength);
	suspensionForce = springForce + damperForce;

	if (!isWeelFree)
	{
		//Use friction on body and update wheel spin
		UpdateForceOnWheel(suspensionForce, deltaTime);
		//Use suspension on body
		FVector suspensionForceVec = scene_damperPointTop->GetForwardVector() * (-suspensionForce);
		data->addForceAtBody(suspensionForceVec, scene_damperPointTop->GetComponentLocation(), NAME_None);

		//Debug info
		DrawDebugLine(GetWorld(), scene_damperPointTop->GetComponentLocation(), scene_damperPointTop->GetComponentLocation() +
			suspensionForceVec / 100.0f, FColor::Blue, false);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "suspensionForceVec " + suspensionForceVec.ToString());
	}

	//set wheel spin velocity
	wheelSpinVelocity -= wheelSpinVelocity * (0.1f * deltaTime);//fake friction on a wheel 
	mesh_wheel->AddRelativeRotation({ 0.0f, 0.0f, wheelSpinVelocity * deltaTime });
			
	//Debug point
	tools::DubugPoint(GetWorld(), scene_damperPointBot->GetComponentLocation(), FColor::Green, "damper bot");
	tools::DubugPoint(GetWorld(), scene_damperPointTop->GetComponentLocation(), FColor::Green, "damper top");			
	tools::DubugPointOnScreen(GetWorld(), wheelCenterLoc, FColor::Green, "wheel center");
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Purple, this->GetName());
}

void USuspensionSide::UpdateForceOnWheel(float suspensionForces, float deltaTime)
{
	FVector frictionForceVec;
	auto tr = scene_wheelCenter->GetComponentTransform();
	auto hitPos = scene_wheelCenter->GetComponentLocation();
	FVector velocityAtHitPoint = mesh_wheel->GetPhysicsLinearVelocityAtPoint(hitPos);
	velocityAtHitPoint = tr.GetRotation().UnrotateVector(velocityAtHitPoint);

	//update frictio force
	float frictioForce = velocityAtHitPoint.X * data->frictionKof * suspensionForce;
	float maxFrictioForce = velocityAtHitPoint.X * data->commonMass;
	if (abs(frictioForce) > abs(maxFrictioForce)) frictioForce = maxFrictioForce;
	frictionForceVec = { -frictioForce, 0.0f, 0.0f };
	frictionForceVec = tr.GetRotation().RotateVector(frictionForceVec);
	data->addForceAtBody(frictionForceVec, hitPos, NAME_None);
	//update wheel spin
	wheelSpinVelocity = velocityAtHitPoint.Y / (data->wheelRadius * 2 * PI) * 360.0f;

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "frictionForceVec " + frictionForceVec.ToString());
	DrawDebugLine(GetWorld(), hitPos, hitPos + frictionForceVec, FColor::Blue, false);//friction force vector	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::White, "velocity " + velocityAtHitPoint.ToString());
	DrawDebugLine(GetWorld(), scene_damperPointTop->GetComponentLocation(), scene_damperPointTop->GetComponentLocation() +
		mesh_wheel->GetPhysicsLinearVelocityAtPoint(hitPos), FColor::White, false);	//wheel velocity
}

