// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIModule/Classes/AIController.h"
#include "MutantAIController.generated.h"

UCLASS()
class FIRSTPERSONSHOOTER_API AMutantAIController : public AAIController
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

private:
	class UNavigationSystemV1* NavArea;
	FVector RandomLocation;

public:
	UFUNCTION()
	void RandomPatrol();
};
