// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MutantEnemy.generated.h"

UCLASS()
class FIRSTPERSONSHOOTER_API AMutantEnemy : public ACharacter
{
	GENERATED_BODY()

public:

	AMutantEnemy();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	bool PlayerDetected;
	bool CanAttackPlayer;

	UPROPERTY(BlueprintReadWrite)
	bool CanDealDamage;

	class AFirstPersonShooterCharacter* PlayerREF;

	UPROPERTY(EditAnywhere)
	class USphereComponent* PlayerCollisionDetection;

	UPROPERTY(EditAnywhere)
	class USphereComponent* PlayerAttackCollisionDetection;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* DamageCollision;

	class AMutantAIController* MutantAIController;

	void OnAIMoveCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);

	UPROPERTY(EditAnywhere)
	float StoppingDistance = 250.0f;

	FTimerHandle SeekPlayerTimerHandle;

	UFUNCTION()
	void MoveToPlayer();

	UFUNCTION()
	void SeekPlayer();

	UFUNCTION()
	void StopSeekingPlayer();

	UFUNCTION()
	void OnPlayerDetectedOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerDetectedOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnPlayerAttackOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDealDamageOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	class UAnimMontage* EnemyAttackAnimation;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* EnemyDyingAnimation;

	class UAnimInstance* AnimInstance;

	UFUNCTION(BlueprintCallable)
	void AttackAnimationEnded();

private:
	
	void DyingMutantEnemy();
	void DestroyActor();

	FTimerHandle DestroyTimerHandle;

	float BaseSphereRadius = 300.0f;
	float AttackSphereRadius = 0.0f;
	float DetectionSphereRadius = 800.0f;
	float AttackDamage = 20.0f;

	bool bAttacking = false;
	bool bAlive = true;
	bool bAnimEnd = false;

public:

	float MutantHP = 1000.0f;
};

