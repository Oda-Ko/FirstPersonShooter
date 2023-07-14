// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "TP_WeaponComponent.h"
#include "FirstPersonShooterCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS()
class AFirstPersonShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

public:
	AFirstPersonShooterCharacter();

protected:
	virtual void BeginPlay();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

protected:

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	void ToggleCrouch();
	void StopCrouching();

	void StartSprint();
	void StopSprint();

	void StartSlide();
	void StopSlide();

public:
	void ADS();
	void StopADS();

protected:

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;


public:

	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	
	virtual void Tick(float DeltaTime) override;

	void DyingCharacter();

	UPROPERTY()
	FTimeline VaultTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Crouch")
	float WalkSpeedCrouched = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Slide")
	float SlideTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Slide")
	float SlideSpeed = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Slide")
	float ChangeSlideSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Sprint")
	float WalkSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Sprint")
	float SprintSpeed = 550.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Sprint")
	float ChangeSprintSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Aim")
	float AimingFOV = 55.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Aim")
	float BaseFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement | Aim")
	float FOVChangeSpeed = 10.0f;

	float DefaultCapsuleHalfHeight;

	bool bHoldingCrouch;
	bool bCrouching;
	bool bHoldingSprint;
	bool bSprinting;
	bool bSliding;
	bool bAiming;
	bool bAlive = true;

	FTimerHandle SlideStop;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float HitPoint = 100.0f;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetHP();

};

