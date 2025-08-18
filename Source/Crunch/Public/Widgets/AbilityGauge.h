// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

class UAbilityToolTip;
struct FOnAttributeChangeData;
struct FGameplayAbilitySpec;
class UAbilitySystemComponent;
class UGameplayAbility;
class UImage;
class UTextBlock;


USTRUCT(BlueprintType)
struct FAbilityWidgetDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

/**
 * 
 */
UCLASS()
class CRUNCH_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	//根据传入的WidgetData配置.
	void ConfigureWithWidgetData(const FAbilityWidgetDataRow* AbilityWidgetData);

	//当能力被提交时调用.
	void OnAbilityCommitted(UGameplayAbility* Ability);

	//当能力被提交时 开始计时 Cooldown.
	void StartCooldown(float CooldownTimeRemaining, float CooldownDuration);

	//Cooldown 完成时调用.
	void CooldownFinished();

	void UpdateCooldown();

	const FGameplayAbilitySpec* GetAbilitySpec();

	void OnAbilitySpecUpdate(const FGameplayAbilitySpec& NewSpec);

	void UpdateCanCast();
	void OnUpgradePointUpdate(const FOnAttributeChangeData& Data);
	void OnManaUpdate(const FOnAttributeChangeData& Data);

	void CreateAbilityToolTip(const FAbilityWidgetDataRow* AbilityWidgetData);
private:

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FName IconMaterialParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FName CooldownPercentParamName = "Percent";
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Icon;

	//控制技能Icon下方代表等级的进度.
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FName AbilityLevelParamName = "Level";

	//控制技能是灰色还是亮色. --- 代表是否可以使用或者是否学习.
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FName CanCastAbilityParamName = "CanCast";

	//控制技能Icon的周围一圈闪烁. ---- 1为闪烁 0为无.
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FName AvailableParamName = "Available";
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LevelGauge;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownCountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownDurationText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CostText;

	UPROPERTY(EditDefaultsOnly, Category = "ToolTip")
	TSubclassOf<UAbilityToolTip> AbilityToolTipClass;
	
	UPROPERTY()
	TWeakObjectPtr<UGameplayAbility> AbilityCDO; // The ability this gauge represents

	//Cooldown的更新间隔.
	UPROPERTY(EditAnywhere, Category = "Data")
	float CooldownUpdateInterval = 0.1f;

	//Cooldown 文本显示的格式化选项.
	FNumberFormattingOptions WholeNumberFormattingOptions;
	FNumberFormattingOptions TwoDigitNumberFormattingOptions;
	
	//Cooldown的总时间和剩余时间.
	float CachedCooldownDuration = 0.f;
	float CachedCooldownRemainingTime = 0.f;

	//Cooldown Finish/Update 的计时器.
	FTimerHandle CooldownTimerHandle;
	FTimerHandle CooldownUpdateTimerHandle;

	UPROPERTY()
	UAbilitySystemComponent* OwnerASC;

	FGameplayAbilitySpecHandle CachedAbilitySpecHandle;

	bool bIsAbilityLearned = false;
};
