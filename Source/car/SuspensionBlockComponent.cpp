#include "SuspensionBlockComponent.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Core/Public/Math/TransformVectorized.h"
#include "Engine.h"
#include "tools/debug.h"

// Sets default values for this component's properties
USuspensionBlockComponent::USuspensionBlockComponent()
	: MeshWheel()
	, SceneWheelCenter()
	, SceneDamperPointTop()
	, SceneDamperPointBot()
	, bIsLeft(false)
	, SpringForce(0.0f)
	, DamperForce(0.0f)
	, SuspensionForce(0.0f)
	, CompressionVelocity(0.0f)
	, WheelSpinVelocity(0.0f)
	, ReyLength(0.0f)
	, CurrDamperLength(0.0f)
	, MaxDamperLength(0.0f)
	, MinDamperLength(0.0f)
	, Data()
	, CollisionParams()
{
	MeshWheel = CreateDefaultSubobject<UStaticMeshComponent>("mesh_wheel");
	SceneWheelCenter = CreateDefaultSubobject<USceneComponent>("wheelCenter");
	SceneDamperPointTop = CreateDefaultSubobject<USceneComponent>("damperPointTop");
	SceneDamperPointBot = CreateDefaultSubobject<USceneComponent>("damperPointBot");

	PrimaryComponentTick.bCanEverTick = true;
	SceneDamperPointTop->SetupAttachment(this);
	SceneDamperPointBot->SetupAttachment(this);
	MeshWheel->SetupAttachment(SceneDamperPointBot);
	SceneWheelCenter->SetupAttachment(SceneDamperPointBot);
	CollisionParams.AddIgnoredActor(this->GetAttachmentRootActor());
}

void USuspensionBlockComponent::Init(const FCommonSuspensionDataPtr& newSuspensionData, bool isLeftSide)
{
	bIsLeft = isLeftSide;
	Data = newSuspensionData;
	CurrDamperLength = Data->RelaxDamperLength;
	MaxDamperLength = Data->RelaxDamperLength + Data->DamperMove;
	MinDamperLength = Data->RelaxDamperLength - Data->DamperMove;
}

void USuspensionBlockComponent::BeginPlay()
{
	Super::BeginPlay();

	//set damper pos
	SceneDamperPointTop->SetRelativeLocation({ 0.0f, 0.0f, Data->RelaxDamperLength });
	SceneDamperPointTop->SetRelativeRotation({ -90.0f, 0.0f, 0.0f }); //TODO: use kpiAngle;
	//set wheel pos
	const auto& LeftDumperTr = SceneDamperPointTop->GetRelativeTransform();
	FVector PosWheel = LeftDumperTr.GetLocation() + LeftDumperTr.GetRotation().Vector() * Data->RelaxDamperLength;
	SceneDamperPointBot->SetRelativeLocation(PosWheel);
	FVector PosWheelCenter;
	if (bIsLeft) {
		PosWheelCenter.X = -Data->WheelWidth / 2.0f;
	} else {
		PosWheelCenter.X = Data->WheelWidth / 2.0f;
	}

	SceneWheelCenter->SetRelativeLocation(PosWheelCenter);
}

void USuspensionBlockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateSuspension(DeltaTime);
}

void USuspensionBlockComponent::TurnWheel(float Angle)
{
	SceneDamperPointBot->SetRelativeRotation({0.0f, Angle, 0.0f});
}

void USuspensionBlockComponent::UpdateSuspension(float DeltaTime)
{
	//rays cast
	const auto& WheelCenterLoc = SceneWheelCenter->GetComponentLocation();
	const auto& WheelCenterRot = SceneWheelCenter->GetComponentRotation();
	bool bIsAnyHit = false;
	float HitDistance = TNumericLimits< float >::Max();
	FVector HitPos;

	const auto& VecToWheelTop = WheelCenterRot.RotateVector(FVector::UpVector);
	const auto& TopPoint = WheelCenterLoc + VecToWheelTop * (Data->WheelRadius);
	{
		FHitResult OutHit;
		for (float Angle = -75.0f; Angle <= 75.0f; Angle += 15.0f)
		{
			FRotator Rotator( 0.0f, 0.0f, Angle );

			const auto& VecToStart = (WheelCenterRot + Rotator).RotateVector(FVector::DownVector);
			const auto& StartPoint = TopPoint + VecToStart * (Data->WheelRadius);
			const auto& EndPoint = StartPoint + VecToWheelTop * -(Data->WheelRadius + Data->DamperMove);

			bool bIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams);
			if (bIsHit && OutHit.bBlockingHit)
			{
				if (OutHit.Distance < HitDistance)
				{
					HitDistance = OutHit.Distance;
					HitPos = OutHit.ImpactPoint;
				}
				bIsAnyHit = true;
			}

			DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Purple, false);// Debug ray cast
		}
	}

	//calc damper motion length
	float MaxMotionLength = MaxDamperLength - CurrDamperLength;
	float MinMotionLength = MinDamperLength - CurrDamperLength;
	if (bIsAnyHit)
	{
		MaxMotionLength = FMath::Max(HitDistance - Data->WheelRadius, MinMotionLength);
		tools::DebugPoint(GetWorld(), HitPos, FColor::Red, "hitPos");
	}
	bool bIsWeelFree = (MaxMotionLength > 0.0f || !bIsAnyHit || HitDistance > Data->WheelRadius);

	float MotionLength;
	if (bIsWeelFree)
	{
		MotionLength = ((SuspensionForce / Data->WheelMass) * DeltaTime * DeltaTime / 2 + CompressionVelocity * DeltaTime);
		if (MotionLength > 0.0f) {// decompression
			if (MotionLength >= MaxMotionLength) {
				MotionLength = MaxMotionLength;
				bIsWeelFree = false;
			}
		} else {//compression
			if (MotionLength < MinMotionLength) MotionLength = MinMotionLength;
		}
	}
	else {
		MotionLength = MaxMotionLength * Data->WheelSpringiness;
	}
	CompressionVelocity = MotionLength / DeltaTime;

	//calc suspension force
	SceneDamperPointBot->AddRelativeLocation(FRotator(-90.f, 0.f, 0.f).Vector() * (MotionLength)); //TODO: use kpiAngle;
	CurrDamperLength += MotionLength;
	DamperForce = Data->Damper * -MotionLength;
	SpringForce = Data->Stiffness * (Data->RelaxDamperLength - CurrDamperLength);
	SuspensionForce = SpringForce + DamperForce;

	if (!bIsWeelFree)
	{
		//Use friction on body and update wheel spin
		UpdateForceOnWheel(SuspensionForce, DeltaTime);
		//Use suspension on body
		FVector SuspensionForceVec = SceneDamperPointTop->GetForwardVector() * (-SuspensionForce);
		Data->AddForceAtBody(SuspensionForceVec, SceneDamperPointTop->GetComponentLocation(), NAME_None);

		//Debug output
		DrawDebugLine(GetWorld(), SceneDamperPointTop->GetComponentLocation(), SceneDamperPointTop->GetComponentLocation() +
			SuspensionForceVec / 100.0f, FColor::Blue, false);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "suspensionForceVec " + SuspensionForceVec.ToString());
		}
	}

	//set wheel spin velocity
	WheelSpinVelocity -= WheelSpinVelocity * (0.1f * DeltaTime);//fake friction on a wheel
	MeshWheel->AddRelativeRotation({ 0.0f, 0.0f, WheelSpinVelocity * DeltaTime });

	//Debug output
	tools::DebugPoint(GetWorld(), SceneDamperPointBot->GetComponentLocation(), FColor::Green, "damper bot");
	tools::DebugPoint(GetWorld(), SceneDamperPointTop->GetComponentLocation(), FColor::Green, "damper top");
	tools::DebugPointOnScreen(GetWorld(), WheelCenterLoc, FColor::Green, "wheel center");
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Purple, this->GetName());
	}
}

void USuspensionBlockComponent::UpdateForceOnWheel(float suspensionForces, float deltaTime)
{	
	FTransform TrWheelCenter = SceneWheelCenter->GetComponentTransform();
	FVector HitPos = SceneWheelCenter->GetComponentLocation();
	FVector VelocityAtHitPoint = MeshWheel->GetPhysicsLinearVelocityAtPoint(HitPos);
	VelocityAtHitPoint = TrWheelCenter.GetRotation().UnrotateVector(VelocityAtHitPoint);

	//update friction force
	float FrictionForce = VelocityAtHitPoint.X * Data->FrictionKof * SuspensionForce;
	float MaxFrictionForce = VelocityAtHitPoint.X * Data->CommonMass;
	if (abs(FrictionForce) > abs(MaxFrictionForce)) FrictionForce = MaxFrictionForce;
	FVector FrictionForceVec = TrWheelCenter.GetRotation().RotateVector(FVector( -FrictionForce, 0.0f, 0.0f ));
	Data->AddForceAtBody(FrictionForceVec, HitPos, NAME_None);

	//update wheel spin
	WheelSpinVelocity = VelocityAtHitPoint.Y / (Data->WheelRadius * 2.0f * PI) * 360.0f;

	//Debug output
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Blue, "frictionForceVec " + FrictionForceVec.ToString());
	DrawDebugLine(GetWorld(), HitPos, HitPos + FrictionForceVec/2.0f, FColor::Blue, false);//friction force vector
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::White, "velocity " + VelocityAtHitPoint.ToString());
	DrawDebugLine(GetWorld(), SceneDamperPointTop->GetComponentLocation(), SceneDamperPointTop->GetComponentLocation() +
		MeshWheel->GetPhysicsLinearVelocityAtPoint(HitPos), FColor::White, false);	//wheel velocity
}

