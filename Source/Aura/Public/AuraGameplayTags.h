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

	void InitializeNativeGameplayTags();

	FGameplayTag Attribute_Secondary_Armor;

private:
	AuraGameplayTags() = default;
	~AuraGameplayTags() = default;
};
