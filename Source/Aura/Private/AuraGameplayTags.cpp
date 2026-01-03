// Copyright Sylar129

#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

AuraGameplayTags& AuraGameplayTags::Get()
{
	static AuraGameplayTags GameplayTags;
	return GameplayTags;
}

void AuraGameplayTags::InitializeNativeGameplayTags()
{
	Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduces damage taken, improves Block Chance"));
}
