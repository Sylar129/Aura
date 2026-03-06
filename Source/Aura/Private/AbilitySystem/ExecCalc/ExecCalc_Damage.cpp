// Copyright Sylar129

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	const ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Get Damage Set by Caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	// Capture BlockChance on Target, and determine if there was a successful Block
	// If Block, halve the damage.
	float BlockChance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, BlockChance);
	BlockChance = FMath::Max(0.f, BlockChance);

	const bool bBlocked = FMath::RandRange(1.0f, 100.0f) < BlockChance;
	Damage = bBlocked ? Damage * 0.5f : Damage;

	float TargetArmor = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max(0.f, TargetArmor);

	float ArmorPenetration = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, ArmorPenetration);
	ArmorPenetration = FMath::Max(0.f, ArmorPenetration);

	float CriticalHitChance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, CriticalHitChance);
	CriticalHitChance = FMath::Max(0.f, CriticalHitChance);

	float CriticalHitResistance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, CriticalHitResistance);
	CriticalHitResistance = FMath::Max(0.f, CriticalHitResistance);

	float CriticalHitDamage = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, CriticalHitDamage);
	CriticalHitDamage = FMath::Max(0.f, CriticalHitDamage);

	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetCharacterLevel());

	FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetCharacterLevel());

	const float EffectiveArmor = TargetArmor *= (100.0f - ArmorPenetration * ArmorPenetrationCoefficient) / 100.0f;
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.0f;


	FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetCharacterLevel());

	bool bCriticalHit = FMath::RandRange(1.0f, 100.0f) < (CriticalHitChance - CriticalHitResistance * CriticalHitResistanceCoefficient);
	Damage = bCriticalHit ? Damage * 2.0f + CriticalHitDamage : Damage;

	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);

	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
