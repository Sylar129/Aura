// Copyright Sylar129

#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(
	UAbilitySystemComponent*	AbilitySystemComponent,
	const FGameplayEffectSpec&	EffectSpec,
	FActiveGameplayEffectHandle ActiveEffectHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("EffectApplied"));
}
