

// Mostly copied and pasted by Mathureswaran

#include "VRCharacter.h"
#include"Components/InputComponent.h"
#include"Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include"Engine/Classes/Camera/PlayerCameraManager.h"
#include"Engine/Classes/GameFramework/PlayerController.h"
#include"Engine/Public/TimerManager.h"
#include"Components/CapsuleComponent.h"
#include"NavigationSystem.h"
#include"Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include"DrawDebugHelpers.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(FName("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(VRRoot);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(FName("Destination Marker"));
	DestinationMarker->SetupAttachment(GetRootComponent());

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(FName("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (BlinkerMaterialBase)
	{
		BlinkerMaterialInstance =  UMaterialInstanceDynamic::Create(BlinkerMaterialBase, this);
		PostProcessComponent->AddOrUpdateBlendable(BlinkerMaterialInstance);
	}

}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FVector::VectorPlaneProject(NewCameraOffset.Z,)
	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);

	UpdateDestinationMarker();
	UpdateBlinkers();


}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Teleport"), IE_Released, this, &AVRCharacter::BeginTeleport);
	//PlayerInputComponent->BindAction(TEXT("Teleport"), this, &AVRCharacter::BeginTeleport);

}

bool AVRCharacter::FindTeleportDestination(FVector & OutLocation)
{
	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector()*MaxTeleportRange;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

	if (!bHit)
	{
		return false;
	}
	FNavLocation NavLocation;
	bool bOnNavMesh = UNavigationSystemV1::GetNavigationSystem(GetWorld())->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectionExtent);

	if (!bOnNavMesh)
	{
		return false;
	}
	/*if (bOnNavMesh && bHit)
	{

	}

	else
	{
		DestinationMarker->SetVisibility(false);
	}*/
	DrawDebugLine(GetWorld(), Start, End, FColor(1, 0, 0), true, 1.0f, 1, 1.0f);

	OutLocation = NavLocation.Location;
	return true;
}

void AVRCharacter::UpdateDestinationMarker()
{
	FVector Location;
	bool bHasDestination = FindTeleportDestination(Location);
	if (bHasDestination)
	{
		DestinationMarker->SetWorldLocation(Location);
		DestinationMarker->SetVisibility(true);
	}
	else
	{
		DestinationMarker->SetVisibility(false);
	}



}
void AVRCharacter::UpdateBlinkers()
{
	if (RadiusVsVelocity == nullptr)
	{
		return;
	}
	float Speed = GetVelocity().Size();
	float Radius = RadiusVsVelocity->GetFloatValue(Speed);

	BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"), Radius);
	FVector2D Center = GetBlinkerCenter();
	//BlinkerMaterialInstance->SetVectorParameterValue(TEXT("Center"), FLinearColor(Center.X,Center.Y,0));
	//BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"),Radius);
}

void AVRCharacter::BeginTeleport()
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC != nullptr)
	{
		StartFade(0, 1);
	}
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::EndTeleport, FadeTime);

}

void AVRCharacter::EndTeleport()
{
	SetActorLocation(DestinationMarker->GetComponentLocation());
	StartFade(1, 0);
}

void AVRCharacter::StartFade(float FromAlpha, float ToAlpha)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{
		PC->PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, FadeTime, FLinearColor::Black);
	}
}


FVector2D AVRCharacter::GetBlinkerCenter()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return FVector2D(0.5f, 0.5f);
	}
	FVector MovementDirection = GetVelocity().GetSafeNormal();
	if (MovementDirection.IsNearlyZero())
	{
		return FVector2D(0.5, 0.5);
	}

	FVector WorldStationaryLocation = Camera->GetComponentLocation() + MovementDirection * 1000.0f;
	FVector2D ScreenStationaryLocation;

	PlayerController->ProjectWorldLocationToScreen(WorldStationaryLocation,ScreenStationaryLocation);

	int SizeX, SizeY;

	PlayerController->GetViewportSize(SizeX,SizeY);
	ScreenStationaryLocation.X = ScreenStationaryLocation.X / SizeX;
	ScreenStationaryLocation.Y = ScreenStationaryLocation.Y / SizeY;

	return ScreenStationaryLocation;
}

void AVRCharacter::MoveForward(float Throttle)
{
	AddMovementInput(Camera->GetForwardVector()*Throttle);
}

void AVRCharacter::MoveRight(float Throttle)
{
	AddMovementInput(Camera->GetRightVector()*Throttle);
}

