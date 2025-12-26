// Copyright Sylar129

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	DoNotApply,
	ApplyOnOverlap,
	ApplyOnEndOverlap,
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	DoNotRemove,
	RemoveOnEndOverlap,
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectRemoval = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy EffectApplicationPolicy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectRemovalPolicy EffectRemovalPolicy;

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
};
