// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	// �ִ� HP
	float maxHP = 100;
	// ���� HP
	float currHP;
	
public:
	virtual void NativeConstruct() override;

	float UpdateHPBar(float damage);
};