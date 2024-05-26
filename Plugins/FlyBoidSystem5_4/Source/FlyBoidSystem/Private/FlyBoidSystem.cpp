// Copyright 2023-2024 LinFengZi. All Rights Reserved.

#include "FlyBoidSystem.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FFlyBoidSystemModule"

void FFlyBoidSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir  = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("FlyBoidSystem"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/MyShaders"), PluginShaderDir );
}

void FFlyBoidSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFlyBoidSystemModule, FlyBoidSystem)