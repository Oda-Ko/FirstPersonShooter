// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonShooter/Enemy/MutantAIController.h"
#include "NavigationSystem.h"

void AMutantAIController::BeginPlay()
{
    Super::BeginPlay();

    NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);

    RandomPatrol();
}

void AMutantAIController::RandomPatrol()
{
    if (NavArea)
    {
        NavArea->K2_GetRandomReachablePointInRadius(GetWorld(), GetPawn()->GetActorLocation(), RandomLocation, 1500.0f);
        MoveToLocation(RandomLocation);
    }
}
