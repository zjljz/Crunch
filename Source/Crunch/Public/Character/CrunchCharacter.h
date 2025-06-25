// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "CrunchCharacter.generated.h"

class UWidgetComponent;
class UCrunchAttributeSet;
class UCrunchAbilitySystemComponent;

UCLASS()
class CRUNCH_API ACrunchCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACrunchCharacter();

	virtual void Tick(float DeltaTime) override;

	//这个函数只在Server上调用.
	virtual void PossessedBy(AController* NewController) override;
	
protected:
	virtual void BeginPlay() override;

public:
	/***************************************************
	 *           Gameplay Ability System
	 **************************************************/

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void ServerInit();
	void ClientInit();

private:
	UPROPERTY(VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UCrunchAbilitySystemComponent> CrunchASC;

	UPROPERTY(VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UCrunchAttributeSet> CrunchAttributeSet;

	/***************************************************
	 *           UI Widgets
	 **************************************************/

	//为OverHeadWidget配置ASC.
	void ConfigureOverHeadWidget();

	//定时 检测是否需要显示头顶UI.
	void UpdateHeadGaugeVisibility();
private:
	UPROPERTY(VisibleAnywhere, Category = "UI | OverHead")
	TObjectPtr<UWidgetComponent> OverHeadWidgetComp;

	//检测头顶UI范围的定时器.
	FTimerHandle OverheadWidgetVisibilityTimer;

	//头顶UI显示检测的间隔.
	UPROPERTY(EditDefaultsOnly, Category = "UI | OverHead")
	float OverHeadWidgetVisibilityCheckGap = 1.f;
	
	//人物头顶UI显示范围. --- 这是使用的是实际距离的平方(10m) 这样更快.
	UPROPERTY(EditDefaultsOnly, Category = "UI | OverHead")
	float OverHeadWidgetVisibilityRange = 1000000.0f;
};
