// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BoidsFlyGameModeBase.generated.h"



UCLASS()
class BOIDSFLY_API ABoidsFlyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ABoidsFlyGameModeBase();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	bool UseGPU = true;
	UFUNCTION(BlueprintCallable)
	void ComputeBoid();
	UFUNCTION(BlueprintCallable)
	void ClearBoidBase();

	
	int32 MaxNearNum = 0;
	FVector FlowTarget;
};
