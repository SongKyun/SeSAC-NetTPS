// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetTPSWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UNetTPSWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	class UImage* imgCrosshair;

	UPROPERTY()
	class UHorizontalBox* bulletMagazine;

    UPROPERTY(meta=(BindWidget))
    class UHealthBar* HealthBar;

	// 총알 위젯 Blueprint
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> bulletFactory;

	virtual void NativeConstruct() override;

	void ShowCrosshair(bool isShow);

	// 총알 UI 추가 함수
	void AddBullet();

	void PopBullet(int32 index);

	// 총알 UI 모두 삭제 함수
	void PopBulletAll();
};
