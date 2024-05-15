// Copyright Epic Games, Inc. All Rights Reserved.


#include "BoidsFlyGameModeBase.h"
#include "BoidsFly.h"
#include "Kismet\GameplayStatics.h"
#include "MyBoid.h"


ABoidsFlyGameModeBase::ABoidsFlyGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABoidsFlyGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (UseGPU)
	{
		ComputeBoid();
	}
}

void ABoidsFlyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABoidsFlyGameModeBase::ComputeBoid()
{
	TArray<AActor*> AllBoid;
	UGameplayStatics::GetAllActorsOfClass(this, AMyBoid::StaticClass(), AllBoid);
	for (int i=0; i<AllBoid.Num(); i++)
	{
		AMyBoid* Bird = Cast<AMyBoid>(AllBoid[i]);
		if (Bird)
		{
			if (FMyBoidModule::Get().BoidInfoSave.BoidBase.Contains(i))
			{
				FMyBoidModule::Get().BoidInfoSave.BoidBase[i].Position = Bird->GetActorLocation();
				FMyBoidModule::Get().BoidInfoSave.BoidBase[i].Velocity = Bird->GetCurVelocity();
			}
		}
	}
	if (AllBoid.Num() == FMyBoidModule::Get().BoidInfoSave.BoidBase.Num())
	{
		ENQUEUE_RENDER_COMMAND(BoidCompute)(
			[](FRHICommandListImmediate& RHICmdList) {
			FMyBoidModule::Get().RunComputeShader(RHICmdList);
		});
		FlushRenderingCommands();
		ENQUEUE_RENDER_COMMAND(BoidResult)(
			[](FRHICommandListImmediate& RHICmdList) {
			FMyBoidModule::Get().GetComputeShaderResult(RHICmdList);
		});
		FlushRenderingCommands();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Boid Num Error!!!!!!AllBoidNum=%d, BoidBaseNum=%d"), AllBoid.Num(), FMyBoidModule::Get().BoidInfoSave.BoidBase.Num());
	}
	
	for (int i = 0; i < AllBoid.Num(); i++)
	{
		AMyBoid* Bird = Cast<AMyBoid>(AllBoid[i]);
		if (Bird)
		{
			Bird->UpdateBird(true);
		}
	}
}

void ABoidsFlyGameModeBase::ClearBoidBase()
{
	FMyBoidModule::Get().BoidInfoSave.BoidBase.Empty();
}
