// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

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
private:

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FName IconMaterialParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FName CooldownPercentParamName = "Percent";
	
	UPROPERTY(meta = (BindWidget))
	UImage* Icon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooldownCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooldownDurationText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CostText;

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
};
