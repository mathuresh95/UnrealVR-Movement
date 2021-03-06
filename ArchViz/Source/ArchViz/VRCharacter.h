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
	bool FindTeleportDestination(FVector &OutLocation);
	void UpdateDestinationMarker();
	void BeginTeleport();
	void EndTeleport();
	void StartFade(float FromAlpha, float ToAplha);
	void UpdateBlinkers();
	FVector2D GetBlinkerCenter();
	void MoveForward(float Throttle);
	void MoveRight(float Throttle);

	UPROPERTY(VisibleAnywhere, Category = Setup)
		class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, Category = Setup)
		class USceneComponent* VRRoot;
	UPROPERTY(VisibleAnywhere, Category = Setup)
		class UStaticMeshComponent* DestinationMarker;
	UPROPERTY(VisibleAnywhere, Category = Setup)
		class UPostProcessComponent* PostProcessComponent;
	UPROPERTY(EditAnywhere, Category = Setup)
		class UMaterialInterface* BlinkerMaterialBase;
	UPROPERTY(EditAnywhere, Category = Setup)
		class UMaterialInstanceDynamic* BlinkerMaterialInstance;
	UPROPERTY(EditAnywhere, Category = Setup)
		class UCurveFloat* RadiusVsVelocity;
	
		

private:
	UPROPERTY(EditAnywhere, Category = Setup)
		float MaxTeleportRange = 500.0f;
	UPROPERTY(EditAnywhere, Category = Setup)
		float FadeTime = 2.0f;
	UPROPERTY(EditAnywhere, Category = Setup)
		FVector TeleportProjectionExtent = FVector(100, 100, 100);


};
