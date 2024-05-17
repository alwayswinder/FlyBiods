// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "MyBoidsManager.generated.h"


class AMyBoid;

struct FMyBoidBase
{
	FMyBoidBase(FVector Pos, FVector Vel) 
	{
		Position = Pos;
		Velocity = Vel;
	}
	FVector Position = FVector(0, 0, 0);
	FVector Velocity = FVector(0, 0, 0);
	FVector Center = FVector(0, 0, 0);
	FVector Flow = FVector(0, 0, 0);
	FVector AovOut = FVector(0, 0, 0);
	int BoidNearNum = 0;
};

struct FMyBoidAttribute
{
	TArray<FMyBoidBase> BoidBase;
	float AovRadius = 20;
	float ViewRadius = 100;
	TArray<AMyBoid*> BoidRef;
};

UCLASS()
class BOIDSFLY_API AMyBoidsManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyBoidsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void RunComputeShader(FRHICommandListImmediate& RHICmdList);
	void GetComputeShaderResult(FRHICommandListImmediate& RHICmdList);

	FMyBoidAttribute BoidInfoSave;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	bool UseGPU = true;
	
	UPROPERTY(BlueprintReadWrite, Category = "Boid")
	FVector GlobalDirection;
	
	UFUNCTION(BlueprintCallable)
	void ComputeBoid();
	UFUNCTION(BlueprintCallable)
	void InitBoidBase(int32 Num);
	
	int32 MaxGroupNum = 0;
	FVector GroupTarget;


private:private:
	FBufferRHIRef BoidBaseBuffer;
	FUnorderedAccessViewRHIRef BoidBaseRecordsUAV;
};
