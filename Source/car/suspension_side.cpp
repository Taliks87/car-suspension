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
	, springForce(0.0f)
	, reyLength(0.0f)	
	, currDamperLength(0.0f)
	, oldDamperLength(0.0f)
{	
	PrimaryComponentTick.bCanEverTick = true;
	//scene_damperPoint->SetupAttachment(this);
}

void USuspensionSide::BeginPlay()
{
	Super::BeginPlay();	
}

void USuspensionSide::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//suspension debug		
	//DrawDebugLine(GetWorld(), GetComponentLocation(), scene_botPointFL->GetComponentLocation(), FColor::Green, false, 1, 0, 1);
	//DrawDebugString(GEngine->GetWorldFromContextObject(this), scene_topPointFL->GetComponentLocation(), *FString::Printf(TEXT("%s[%d]"), TEXT("test"), 12345), NULL, drawColor, drawDuration, drawShadow);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, scene_topPointFL->GetComponentLocation().ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, GetRelativeTransform().GetLocation().ToString());
	//DrawDebugLine(GetWorld(), scene_wheelFL->GetComponentLocation(), scene_wheelFR->GetComponentLocation(), FColor::Green, false, 1, 0, 1);			
}


