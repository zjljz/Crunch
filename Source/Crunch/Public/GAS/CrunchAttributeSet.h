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
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldVal);



private:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;

	UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
};
