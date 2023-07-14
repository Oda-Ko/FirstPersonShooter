// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPersonShooterCharacter.h"
#include "FirstPersonShooterProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AFirstPersonShooterCharacter::AFirstPersonShooterCharacter()
{

	bHasRifle = false;
	
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void AFirstPersonShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}


	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
}

void AFirstPersonShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AFirstPersonShooterCharacter::ToggleCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AFirstPersonShooterCharacter::StopCrouching);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFirstPersonShooterCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFirstPersonShooterCharacter::Look);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AFirstPersonShooterCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFirstPersonShooterCharacter::StopSprint);
	}
}


void AFirstPersonShooterCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFirstPersonShooterCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFirstPersonShooterCharacter::ToggleCrouch()
{
	bHoldingCrouch = true;
	
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		bCrouching = true;
		Crouch();

		if (bSprinting)
		{
			StartSlide();
		}
	}
	
}

void AFirstPersonShooterCharacter::StopCrouching()
{
	bHoldingCrouch = false;
	bCrouching = false;
	UnCrouch();

	if (bSliding)
	{
		StopSlide();
	}
}

void AFirstPersonShooterCharacter::StartSprint()
{
	bHoldingSprint = true;
	if (GetCharacterMovement()->IsMovingOnGround() && !bHoldingCrouch && GetCharacterMovement()->IsMovingOnGround())
	{
		bSprinting = true;
	}
}

void AFirstPersonShooterCharacter::StopSprint()
{
	bHoldingSprint = false;
	bSprinting = false;
}

void AFirstPersonShooterCharacter::StartSlide()
{
	bSprinting = false;
	bSliding = true;
	GetCharacterMovement()->MaxAcceleration = 200.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 200.0f;
	GetCharacterMovement()->GroundFriction = 1.0f;
	GetWorldTimerManager().SetTimer(SlideStop, this, &AFirstPersonShooterCharacter::StopSlide, SlideTime, false, SlideTime);
}

void AFirstPersonShooterCharacter::StopSlide()
{
	bSliding = false;
	bSprinting = (bHoldingSprint) ? true : false;
	bHoldingCrouch = false;
	bCrouching = false;
	UnCrouch();

	GetCharacterMovement()->MaxAcceleration = 2048.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;
	GetCharacterMovement()->GroundFriction = 8.0f;
	GetWorldTimerManager().ClearTimer(SlideStop);
}

void AFirstPersonShooterCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AFirstPersonShooterCharacter::GetHasRifle()
{
	return bHasRifle;
}

void AFirstPersonShooterCharacter::ADS()
{
	bAiming = true;
}

void AFirstPersonShooterCharacter::StopADS()
{
	bAiming = false;
}

void AFirstPersonShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TargetSprintSpeed = (bSprinting) ? SprintSpeed : WalkSpeed;
	const float NewMaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, TargetSprintSpeed, DeltaTime, ChangeSprintSpeed);
	GetCharacterMovement()->MaxWalkSpeed = NewMaxWalkSpeed;

	const float TargetSlideSpeed = (bSliding) ? SlideSpeed : WalkSpeedCrouched;
	const float NewMaxCrochSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeedCrouched, TargetSlideSpeed, DeltaTime, ChangeSlideSpeed);
	GetCharacterMovement()->MaxWalkSpeedCrouched = NewMaxCrochSpeed;

	const float TargetFOV = (bAiming) ? AimingFOV : BaseFOV;
	const float NextFOV = FMath::FInterpTo(FirstPersonCameraComponent->FieldOfView, TargetFOV, DeltaTime, FOVChangeSpeed);
	FirstPersonCameraComponent->SetFieldOfView(NextFOV);

	if (HitPoint<=0.0f && bAlive)
	{
		bAlive = false;
		DyingCharacter();
	}
}

void AFirstPersonShooterCharacter::DyingCharacter()
{
	SetHasRifle(false);
	Destroyed();
}

float AFirstPersonShooterCharacter::GetHP()
{
	return HitPoint;
}
