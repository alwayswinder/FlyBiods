// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


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
	TMap<int, FMyBoidBase> BoidBase;
	float AovRadius = 20;
	float ViewRadius = 100;
};

class FMyBoidModule : public IModuleInterface
{
public:
	static inline FMyBoidModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FMyBoidModule>("BoidsFly");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("BoidsFly");
	}
	void RunComputeShader(FRHICommandListImmediate& RHICmdList);
	void GetComputeShaderResult(FRHICommandListImmediate& RHICmdList);


	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	FMyBoidAttribute BoidInfoSave;
	
private:
	FBufferRHIRef BoidBaseBuffer;
	FUnorderedAccessViewRHIRef BoidBaseRecordsUAV;
};

