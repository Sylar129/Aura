// Copyright Sylar129

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */
class AURA_API AuraGameplayTags
{
public:
	static AuraGameplayTags& Get();

	static void InitializeNativeGameplayTags();

private:
	AuraGameplayTags() = default;
	~AuraGameplayTags() = default;
};
