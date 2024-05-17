// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyBoid.generated.h"


class AMyBoidsManager;

UCLASS()
class BOIDSFLY_API AMyBoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyBoid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FVector GetCurVelocity();
	bool GetIsCollosion();
	void UpdateBird(bool UseComputeShader);
	/*Value*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	int BirdId = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	int32 MinGroupNum = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float SpeedMin = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float SpeedMax = 1.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float ViewRadius = 100.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float AovRadius = 20.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float CenterWeight = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float FlowWeight = 1.2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float AovWeight = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float FreeWeight = 0.9;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float CollosionWeight = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float leaveWallWeight = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float MaxActiveAreaSize = 5000.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float LeaveTime = 0.5;
	
	UPROPERTY(BlueprintReadWrite, Category = "Setting")
	FVector SpawnLocation;
	
	UFUNCTION(BlueprintCallable, Category = "Setting")
	void AddSelfToManage(AMyBoidsManager* InBoidsManager);
	
	UPROPERTY(BlueprintReadWrite, Category = "Setting")
	AMyBoidsManager* BoidsManager;

private:
	/*Value*/
	FVector CurVelocity;
	bool IsCollision = false;
	TArray<FVector> RaysVectors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesBird;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesWall;
	TArray<AActor*> IgnoryActors;
	FTimerHandle CollisionTimer;
	FVector Aov;
	FVector CurAcceleration;

	bool GetRaysVectors();
	void SetIsCollosionFalse();
	FVector ClampPos(FVector Pos);

};
