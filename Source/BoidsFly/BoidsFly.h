// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


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

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

