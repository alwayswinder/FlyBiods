// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBoidsManager.h"
#include "RenderGraphUtils.h"
//#include "ShaderParameterUtils.h"
//#include "RHIUtilities.h"
//#include "DataDrivenShaderPlatformInfo.h"

class FMyBoidComputeShader : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FMyBoidComputeShader)
	SHADER_USE_PARAMETER_STRUCT(FMyBoidComputeShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(int, NumTotal)
		SHADER_PARAMETER(float, AovRadius)
		SHADER_PARAMETER(float, ViewRadius)
		SHADER_PARAMETER_UAV(RWBuffer<FMyBoidBase>, BoidBaseParam)
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
	if (UseGPU)
	{
		ComputeBoid();
	}
}

void AMyBoidsManager::ComputeBoid()
{
	if (BoidInfoSave.BoidBase.Num() >= 1)
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
}

void AMyBoidsManager::InitBoidBase(int32 Num)
{
	BoidInfoSave.BoidBase.Empty();
	for (int i=0; i<=Num; i++)
	{
		BoidInfoSave.BoidBase.Add(FMyBoidBase(FVector(), FVector()));
	}
}

void AMyBoidsManager::RunComputeShader(FRHICommandListImmediate& RHICmdList)
{
	TResourceArray<FMyBoidBase> InitialBoidBaseParams;

	for (int i=0; i<BoidInfoSave.BoidBase.Num(); i++)
	{
		InitialBoidBaseParams.Add(BoidInfoSave.BoidBase[i]);
	}

	//BoidInputBuffer.Initialize(sizeof(FMyBoidInput), 10, PF_Unknown, BUF_UnorderedAccess | BUF_SourceCopy, TEXT("BoidInputBuffer"), &InitialInputParams);
	
	FRHIResourceCreateInfo CreateInfoBoidBase(TEXT("BoidCompute"), &InitialBoidBaseParams);

	BoidBaseBuffer = RHICmdList.CreateStructuredBuffer(sizeof(FMyBoidBase), sizeof(FMyBoidBase) * BoidInfoSave.BoidBase.Num(), BUF_UnorderedAccess | BUF_ShaderResource, CreateInfoBoidBase);
	BoidBaseRecordsUAV = RHICmdList.CreateUnorderedAccessView(BoidBaseBuffer, false, false);

	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidFly_ComputeShader); // Used to gather CPU profiling data for the UE4 session frontend

	TShaderMapRef<FMyBoidComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	RHICmdList.Transition(FRHITransitionInfo(BoidBaseRecordsUAV, ERHIAccess::UAVCompute, ERHIAccess::SRVMask | ERHIAccess::CopySrc));

	FMyBoidComputeShader::FParameters PassParameters;
	PassParameters.BoidBaseParam = BoidBaseRecordsUAV;
	PassParameters.NumTotal = BoidInfoSave.BoidBase.Num();
	PassParameters.AovRadius = BoidInfoSave.AovRadius;
	PassParameters.ViewRadius = BoidInfoSave.ViewRadius;

	FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, PassParameters, FIntVector(BoidInfoSave.BoidBase.Num(), 1, 1));
}

void AMyBoidsManager::GetComputeShaderResult(FRHICommandListImmediate& RHICmdList)
{
	FMyBoidBase* Buffer = (FMyBoidBase*)RHICmdList.LockBuffer(BoidBaseBuffer, 0, sizeof(FMyBoidBase) * BoidInfoSave.BoidBase.Num(), EResourceLockMode::RLM_ReadOnly);
	for (int i = 0; i < BoidInfoSave.BoidBase.Num(); i++)
	{
		BoidInfoSave.BoidBase[i] = Buffer[i];
	}
	RHICmdList.UnlockBuffer(BoidBaseBuffer);
}