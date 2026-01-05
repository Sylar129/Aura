// Copyright Sylar129

#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (auto& [Tag, AttributeGetter] : AuraAttributeSet->TagsToAttributes)
	{
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
		Info.AttributeValue = AttributeGetter().GetNumericValue(AuraAttributeSet);
		AttributeInfoDelegate.Broadcast(Info);
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (auto& [Tag, AttributeGetter] : AuraAttributeSet->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeGetter()).AddLambda([this, Tag](const FOnAttributeChangeData& Data) {
			FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
			Info.AttributeValue = Data.NewValue;
			AttributeInfoDelegate.Broadcast(Info);
		});
	}
}
