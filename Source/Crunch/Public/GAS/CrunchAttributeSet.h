// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CrunchAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	//这个宏自动生成四个函数 分别是获取FGameplayAttribute, 获取CurVal, 设置BaseVal, 初始化Base和Cur
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchAttributeSet, Health)
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchAttributeSet, Mana)
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchAttributeSet, MaxMana)

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


	//这个函数一般用来限制属性的范围 但是某些情况下 会不触发该函数 还需要再次限制.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override; 

	//因为GE的Execution 不会触发PreAttributeChange 所以需要在这里再次限制属性范围
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;

	UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldVal);
};
