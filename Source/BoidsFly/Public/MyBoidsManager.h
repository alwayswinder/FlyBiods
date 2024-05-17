// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "MyBoidsManager.generated.h"


class AMyBoid;

struct FMyBoidBase
{
	FMyBoidBase(FVector3f Pos, FVector3f Vel) 
	{
		Position = Pos;
		Velocity = Vel;
	}
	FVector3f Position = FVector3f(0.f, 0.f, 0.f);
	FVector3f Velocity = FVector3f(0, 0, 0);
	FVector3f Center = FVector3f(0, 0, 0);
	FVector3f Flow = FVector3f(0, 0, 0);
	FVector3f AovOut = FVector3f(0, 0, 0);
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

	//if use compute shader?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	bool UseCS = true;
	
	UPROPERTY(BlueprintReadWrite, Category = "Boid")
	FVector GlobalDirection;
	
	UFUNCTION(BlueprintCallable)
	void ComputeBoid();
	UFUNCTION(BlueprintCallable)
	void InitBoidBase(int32 Num);
	
	int32 MaxGroupNum = 0;
	FVector GroupTarget;
	
	UPROPERTY(BlueprintReadWrite, Category = "Boid")
	bool bIsReady = false;
	

private:
	FBufferRHIRef BoidBaseBuffer;
	FUnorderedAccessViewRHIRef BoidBaseRecordsUAV;
};
