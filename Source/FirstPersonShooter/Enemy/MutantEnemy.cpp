// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonShooter/Enemy/MutantEnemy.h"
#include "FirstPersonShooter/FirstPersonShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "MutantAIController.h"

#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FirstPersonShooter\FirstPersonShooterProjectile.h"


AMutantEnemy::AMutantEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerCollisionDetection = CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Detection"));
	PlayerCollisionDetection->SetupAttachment(RootComponent);

	PlayerAttackCollisionDetection = CreateDefaultSubobject<USphereComponent>(TEXT("Player Attack Collision Detection"));
	PlayerAttackCollisionDetection->SetupAttachment(RootComponent);

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
	DamageCollision->SetupAttachment(GetMesh(), TEXT("LeftHandSocket"));

	PlayerCollisionDetection->SetSphereRadius(DetectionSphereRadius);
	PlayerAttackCollisionDetection->SetSphereRadius(BaseSphereRadius);
}

void AMutantEnemy::BeginPlay()
{
	Super::BeginPlay();

	MutantAIController = Cast<AMutantAIController>(GetController());
	MutantAIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AMutantEnemy::OnAIMoveCompleted);

	PlayerCollisionDetection->OnComponentBeginOverlap.AddDynamic(this, &AMutantEnemy::OnPlayerDetectedOverlapBegin);

	PlayerCollisionDetection->OnComponentEndOverlap.AddDynamic(this, &AMutantEnemy::OnPlayerDetectedOverlapEnd);

	PlayerAttackCollisionDetection->OnComponentBeginOverlap.AddDynamic(this, &AMutantEnemy::OnPlayerAttackOverlapBegin);

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AMutantEnemy::OnDealDamageOverlapBegin);

	AnimInstance = GetMesh()->GetAnimInstance();
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void AMutantEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MutantHP == 0.0f && bAlive)
	{
		bAlive = false;
		DyingMutantEnemy();
	}
}

void AMutantEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMutantEnemy::OnAIMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (!PlayerDetected && bAlive)
	{
		MutantAIController->RandomPatrol();
	}
	else if (PlayerDetected && CanAttackPlayer && bAlive && !bAnimEnd)
	{
		StopSeekingPlayer();
		AnimInstance->Montage_Play(EnemyAttackAnimation);
		PlayerAttackCollisionDetection->SetSphereRadius(AttackSphereRadius);
	}
	else if(PlayerDetected && !CanAttackPlayer && bAlive && bAnimEnd)
	{
		bAnimEnd = false;
		PlayerAttackCollisionDetection->SetSphereRadius(BaseSphereRadius);
		SeekPlayer();
	}
}

void AMutantEnemy::MoveToPlayer()
{
	if (bAlive)
	{
		MutantAIController->MoveToLocation(PlayerREF->GetActorLocation(), StoppingDistance, true);
	}
	else
	{
		MutantAIController->MoveToLocation(GetActorLocation(), StoppingDistance, true);
	}
}

void AMutantEnemy::SeekPlayer()
{
	MoveToPlayer();
	if (!CanAttackPlayer)
	{
		GetWorld()->GetTimerManager().SetTimer(SeekPlayerTimerHandle, this, &AMutantEnemy::SeekPlayer, 0.25f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(SeekPlayerTimerHandle, this, &AMutantEnemy::SeekPlayer, 1.5f, true);
	}
}

void AMutantEnemy::StopSeekingPlayer()
{
	GetWorld()->GetTimerManager().ClearTimer(SeekPlayerTimerHandle);
}

void AMutantEnemy::OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFirstPersonShooterCharacter* Ref = Cast<AFirstPersonShooterCharacter>(OtherActor))
	{
		PlayerREF = Cast<AFirstPersonShooterCharacter>(OtherActor);
		if (PlayerREF && bAlive)
		{
			if (DetectionSphereRadius == 800.0f)
			{
				DetectionSphereRadius = 1200.0f;
				PlayerCollisionDetection->SetSphereRadius(DetectionSphereRadius);
			}
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			PlayerDetected = true;
			SeekPlayer();
		}
	}
}

void AMutantEnemy::OnPlayerDetectedOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AFirstPersonShooterCharacter* Ref = Cast<AFirstPersonShooterCharacter>(OtherActor))
	{
		PlayerREF = Cast<AFirstPersonShooterCharacter>(OtherActor);
		if (PlayerREF && bAlive)
		{
			GetCharacterMovement()->MaxWalkSpeed = 300.0f;
			PlayerDetected = false;
			StopSeekingPlayer();
			MutantAIController->RandomPatrol();
		}
	}
}

void AMutantEnemy::OnPlayerAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFirstPersonShooterCharacter* Ref = Cast<AFirstPersonShooterCharacter>(OtherActor))
	{
		PlayerREF = Cast<AFirstPersonShooterCharacter>(OtherActor);
		if (PlayerREF && bAlive)
		{
			CanAttackPlayer = true;
		}
	}
}

void AMutantEnemy::OnDealDamageOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFirstPersonShooterCharacter* Ref = Cast<AFirstPersonShooterCharacter>(OtherActor))
	{
		PlayerREF = Cast<AFirstPersonShooterCharacter>(OtherActor);
		if (PlayerREF && CanDealDamage && bAlive)
		{
			PlayerREF->HitPoint -= AttackDamage;
		}
	}
}

void AMutantEnemy::DyingMutantEnemy()
{
	AnimInstance->Montage_Play(EnemyDyingAnimation);
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AMutantEnemy::DestroyActor, 1.5f, false);
}

void AMutantEnemy::DestroyActor()
{
	ClearComponentOverlaps();
	Destroy();
}

void AMutantEnemy::AttackAnimationEnded()
{
	CanAttackPlayer = false;
	bAnimEnd = true;
	MutantAIController->MoveToLocation(GetActorLocation(), StoppingDistance, true);
}