// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/AbilitySystem/CrunchAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AbilitySystemGenericDataAsset.h"
#include "Crunch/CrunchGameplayTags.h"
#include "Crunch/Public/AbilitySystem/CrunchAttributeSet.h"
#include "Crunch/Public/AbilitySystem/CrunchHeroAttributeSet.h"

UCrunchAbilitySystemComponent::UCrunchAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCrunchAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
	GetGameplayAttributeValueChangeDelegate(UCrunchAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaChanged);

	GenericConfirmInputID = (int32)ECrunchAbilityInputID::Confirm;
	GenericCancelInputID = (int32)ECrunchAbilityInputID::Cancel;
}

void UCrunchAbilitySystemComponent::InitBaseAttribute()
{
	if (!DA_AbilitySystemGeneric || !DA_AbilitySystemGeneric->GetBaseStatDataTable() || !GetOwner()) return;

	const FHeroBaseStats* BaseStats = nullptr;

	const UDataTable* BaseStatDataTable = DA_AbilitySystemGeneric->GetBaseStatDataTable();
	
	for (const TPair<FName, uint8*>& Pair : BaseStatDataTable->GetRowMap())
	{
		BaseStats = BaseStatDataTable->FindRow<FHeroBaseStats>(Pair.Key, "");
		if (BaseStats && BaseStats->Class == GetOwner()->GetClass())
		{
			break;
		}
	}

	if (BaseStats)
	{
		SetNumericAttributeBase(UCrunchHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);
		SetNumericAttributeBase(UCrunchHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);
		SetNumericAttributeBase(UCrunchHeroAttributeSet::GetStrengthGrowRateAttribute(), BaseStats->StrengthGrowRate);
		SetNumericAttributeBase(UCrunchHeroAttributeSet::GetIntelligenceGrowRateAttribute(), BaseStats->IntelligenceGrowRate);

		SetNumericAttributeBase(UCrunchAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);
		SetNumericAttributeBase(UCrunchAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);
		SetNumericAttributeBase(UCrunchAttributeSet::GetAttackDamageAttribute(), BaseStats->BaseAttackDamage);
		SetNumericAttributeBase(UCrunchAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);
		SetNumericAttributeBase(UCrunchAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);
	}
}

void UCrunchAbilitySystemComponent::ServerSideInit()
{
	InitBaseAttribute();
	GiveInitialAbilities();
	ApplyInitialEffects();
}

void UCrunchAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !DA_AbilitySystemGeneric)
	{
		return;
	}

	for (auto GEClass : DA_AbilitySystemGeneric->GetInitialEffects())
	{
		if (GEClass)
		{
			FGameplayEffectContextHandle GEContextHandle = MakeEffectContext();
			FGameplayEffectSpecHandle GESpec = MakeOutgoingSpec(GEClass, 1.0f, GEContextHandle);
			ApplyGameplayEffectSpecToSelf(*GESpec.Data.Get());
		}
	}
}

void UCrunchAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	for (auto AbilityPair : BaseAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, (int32)AbilityPair.Key, nullptr));
	}

	for (auto AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr));
	}

	for (auto Ability : DA_AbilitySystemGeneric->GetPassiveAbilities())
	{
		GiveAbility(FGameplayAbilitySpec(Ability, 1, -1, nullptr));
	}
}

void UCrunchAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> ApplyGE, int32 Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingSpec(ApplyGE, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*GESpecHandle.Data.Get());
	}
}

void UCrunchAbilitySystemComponent::ApplyFullStat()
{
	AuthApplyGameplayEffect(DA_AbilitySystemGeneric->GetFullStatEffect());
}

void UCrunchAbilitySystemComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	bool bFound = false;
	float MaxHealth = GetGameplayAttributeValue(UCrunchAttributeSet::GetMaxHealthAttribute(), bFound);;

	bool HasTag = HasMatchingGameplayTag(CrunchGameplayTags::Stats_Health_Full);
	if (bFound)
	{
		if (Data.NewValue >= MaxHealth)
		{
			if (!HasTag)
			{
				AddLooseGameplayTag(CrunchGameplayTags::Stats_Health_Full);
			}
		}
		else
		{
			if (HasTag)
			{
				RemoveLooseGameplayTag(CrunchGameplayTags::Stats_Health_Full);
			}
		}
	}

	HasTag = HasMatchingGameplayTag(CrunchGameplayTags::Stats_Health_Empty);
	if (Data.NewValue <= 0.0f)
	{
		if (!HasTag)
		{
			//这个函数添加的Tag不会被复制到Client
			AddLooseGameplayTag(CrunchGameplayTags::Stats_Health_Empty, 1);

			if (DA_AbilitySystemGeneric->GetDeathEffect())
			{
				AuthApplyGameplayEffect(DA_AbilitySystemGeneric->GetDeathEffect());

				FGameplayEventData DeadEventData;
				if (Data.GEModData) DeadEventData.ContextHandle = Data.GEModData->EffectSpec.GetEffectContext();

				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), CrunchGameplayTags::Stats_Dead, DeadEventData);
			}
		}
	}
	else
	{
		if (HasTag)
		{
			RemoveLooseGameplayTag(CrunchGameplayTags::Stats_Health_Empty);
		}
	}
}

void UCrunchAbilitySystemComponent::OnManaChanged(const FOnAttributeChangeData& Data)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	bool bFound = false;
	float MaxMana = GetGameplayAttributeValue(UCrunchAttributeSet::GetMaxManaAttribute(), bFound);;

	bool HasTag = HasMatchingGameplayTag(CrunchGameplayTags::Stats_Mana_Full);
	if (bFound)
	{
		if (Data.NewValue >= MaxMana)
		{
			if (!HasTag)
			{
				AddLooseGameplayTag(CrunchGameplayTags::Stats_Mana_Full);
			}
		}
		else
		{
			if (HasTag)
			{
				RemoveLooseGameplayTag(CrunchGameplayTags::Stats_Mana_Full);
			}
		}
	}

	HasTag = HasMatchingGameplayTag(CrunchGameplayTags::Stats_Mana_Empty);
	if (Data.NewValue <= 0.0f)
	{
		if (!HasTag)
		{
			//这个函数添加的Tag不会被复制到Client
			AddLooseGameplayTag(CrunchGameplayTags::Stats_Mana_Empty, 1);
		}

		if (DA_AbilitySystemGeneric->GetDeathEffect())
		{
			AuthApplyGameplayEffect(DA_AbilitySystemGeneric->GetDeathEffect());
		}
	}
	else
	{
		if (HasTag)
		{
			RemoveLooseGameplayTag(CrunchGameplayTags::Stats_Mana_Empty);
		}
	}
}
