// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include"Engine/Classes/Camera/CameraComponent.h"
#include "VRCharacter.generated.h"
UCLASS()
class ARCHVIZ_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void UpdateDestinationMarker();
	void MoveForward(float Throttle);
	void MoveRight(float Throttle);

private:
	UPROPERTY(VisibleAnywhere,Category = Setup)
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, Category = Setup)
	class USceneComponent* VRRoot;
	UPROPERTY(VisibleAnywhere, Category = Setup)
	class UStaticMeshComponent* DestinationMarker;
	UPROPERTY(EditAnywhere, Category = Setup)
		float MaxTeleportRange = 4000.0f;

};
