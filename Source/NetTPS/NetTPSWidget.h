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

	// ÃÑ¾Ë À§Á¬ Blueprint
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> bulletFactory;

	virtual void NativeConstruct() override;

	void ShowCrosshair(bool isShow);

	// ÃÑ¾Ë UI Ãß°¡ ÇÔ¼ö
	void AddBullet();

	void PopBullet(int32 index);

	// ÃÑ¾Ë UI ¸ðµÎ »èÁ¦ ÇÔ¼ö
	void PopBulletAll();
};
