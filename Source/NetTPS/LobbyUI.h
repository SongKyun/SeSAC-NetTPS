// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(meta=(BindWidget))
    class UButton* Btn_Create;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* Edit_DisplayName;
};
