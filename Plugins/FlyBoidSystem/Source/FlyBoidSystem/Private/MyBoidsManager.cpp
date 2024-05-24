// Copyright 2023-2024 LinFengZi. All Rights Reserved.

#include "MyBoidsManager.h"

#include "DataDrivenShaderPlatformInfo.h"
#include "MyBoid.h"
#include "RenderGraphUtils.h"

class FMyBoidComputeShader : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FMyBoidComputeShader)
	SHADER_USE_PARAMETER_STRUCT(FMyBoidComputeShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(int, NumTotal)
		SHADER_PARAMETER(float, AovRadius)
		SHADER_PARAMETER(float, ViewRadius)
		SHADER_PARAMETER(FVector3f, GlobalDir)
		SHADER_PARAMETER_UAV(RWBuffer<FMyBoidInfo>, BoidInfo)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

IMPLEMENT_SHADER_TYPE(, FMyBoidComputeShader, TEXT("/MyShaders/BirdCompute.usf"), TEXT("MainCS"), SF_Compute);

// Sets default values
AMyBoidsManager::AMyBoidsManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyBoidsManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyBoidsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MaxGroupNum--;
	if (UseCS && bIsReady)
	{
		ComputeBoid();
	}
}

void AMyBoidsManager::ComputeBoid()
{
	for (auto Bird : BoidInfoSave.BoidRef)
	{
		if(Bird)
		{
			BoidInfoSave.BoidInfo[Bird->BirdId].Position = FVector3f(Bird->GetActorLocation());
			BoidInfoSave.BoidInfo[Bird->BirdId].Velocity = FVector3f(Bird->GetCurVelocity());
		}
	}
	if (BoidInfoSave.BoidInfo.Num() >= 1)
	{
		ENQUEUE_RENDER_COMMAND(BoidCompute)(
			[this](FRHICommandListImmediate& RHICmdList) {
			this->RunComputeShader(RHICmdList);
		});
		FlushRenderingCommands();
		
		ENQUEUE_RENDER_COMMAND(BoidResult)(
			[this](FRHICommandListImmediate& RHICmdList) {
			this->GetComputeShaderResult(RHICmdList);
		});
		FlushRenderingCommands();
	}
	for (auto Bird : BoidInfoSave.BoidRef)
	{
		if(Bird)
		{
			Bird->UpdateBird();
		}
	}
}

void AMyBoidsManager::InitBoidBase(int32 Num)
{
	BoidInfoSave.BoidInfo.Empty();
	for (int i=0; i<Num; i++)
	{
		BoidInfoSave.BoidInfo.Add(FMyBoidInfo());
		BoidInfoSave.BoidRef.Add(nullptr);
	}
}

void AMyBoidsManager::RunComputeShader(FRHICommandListImmediate& RHICmdList)
{
	//in
	TResourceArray<FMyBoidInfo> InitialBoidInParams;

	for (int i=0; i<BoidInfoSave.BoidInfo.Num(); i++)
	{
		InitialBoidInParams.Add(BoidInfoSave.BoidInfo[i]);
	}
	FRHIResourceCreateInfo CreateInfoBoid(TEXT("BoidCompute"), &InitialBoidInParams);
	BoidInfoBuffer = RHICmdList.CreateStructuredBuffer(sizeof(FMyBoidInfo), sizeof(FMyBoidInfo) * BoidInfoSave.BoidInfo.Num(), BUF_UnorderedAccess | BUF_ShaderResource, CreateInfoBoid);
	BoidInfoRecordsUAV = RHICmdList.CreateUnorderedAccessView(BoidInfoBuffer, false, false);
	
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidFly_ComputeShader); // Used to gather CPU profiling data for the UE4 session frontend

	TShaderMapRef<FMyBoidComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	RHICmdList.Transition(FRHITransitionInfo(BoidInfoRecordsUAV, ERHIAccess::UAVCompute, ERHIAccess::SRVMask | ERHIAccess::CopySrc));

	FMyBoidComputeShader::FParameters PassParameters;
	PassParameters.NumTotal = BoidInfoSave.BoidInfo.Num();
	PassParameters.AovRadius = BoidInfoSave.AovRadius;
	PassParameters.ViewRadius = BoidInfoSave.ViewRadius;
	PassParameters.GlobalDir = FVector3f(GlobalDirection);
	PassParameters.BoidInfo = BoidInfoRecordsUAV;
	FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, PassParameters, FIntVector(BoidInfoSave.BoidInfo.Num(), 1, 1));
}

void AMyBoidsManager::GetComputeShaderResult(FRHICommandListImmediate& RHICmdList)
{
	FMyBoidInfo* Buffer = (FMyBoidInfo*)RHICmdList.LockBuffer(BoidInfoBuffer, 0, sizeof(FMyBoidInfo) * BoidInfoSave.BoidInfo.Num(), EResourceLockMode::RLM_ReadOnly);
	for (int i = 0; i < BoidInfoSave.BoidInfo.Num(); i++)
	{
		BoidInfoSave.BoidInfo[i].Acceleration = Buffer[i].Acceleration;
	}
	RHICmdList.UnlockBuffer(BoidInfoBuffer);
}