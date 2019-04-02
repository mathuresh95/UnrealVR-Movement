// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCharacter.h"
#include"Components/InputComponent.h"
#include"Components/StaticMeshComponent.h"
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
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

	
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("Forward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("Right"), this, &AVRCharacter::MoveRight);

}

void AVRCharacter::UpdateDestinationMarker()
{
	FHitResult HitResult;
	FVector Start= Camera->GetComponentLocation() ;
	FVector End = Start + Camera->GetForwardVector()*MaxTeleportRange;

	bool bHit =  GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (bHit)
	{
		DestinationMarker->SetWorldLocation(HitResult.Location);
	}
	DrawDebugLine(GetWorld(), Start, End, FColor(1, 0, 0), true, 1.0f, 1, 1.0f);
}

void AVRCharacter::MoveForward(float Throttle)
{
	AddMovementInput(Camera->GetForwardVector()*Throttle);
}

void AVRCharacter::MoveRight(float Throttle)
{
	AddMovementInput(Camera->GetRightVector()*Throttle);
}

