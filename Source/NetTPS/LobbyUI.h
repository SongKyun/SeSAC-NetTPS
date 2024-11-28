#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

UCLASS()
class NETTPS_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
	
public:
    UPROPERTY(meta = (BindWidget))
    class UWidgetSwitcher* WidgetSwitcher;

    UPROPERTY(meta=(BindWidget))
    class UButton* Btn_Go_Create;
    UFUNCTION()
    void GoCreate();

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Go_Find;
    UFUNCTION()
    void GoFind();

    UPROPERTY()
    class UNetGameInstance* gi;

    UPROPERTY(meta=(BindWidget))
    class UButton* Btn_Create;
    UFUNCTION()
    void CreateSession();

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* Edit_DisplayName;

    UPROPERTY(meta = (BindWidget))
    class USlider* Slider_PlayerCount;
    UFUNCTION()
    void OnValueChanged(float value);

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* text_PlayerCount;
    
    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_FindSession;

    UPROPERTY(meta = (BindWidget))
    class UScrollBox* Scroll_SessionList;

    UPROPERTY(EditAnywhere)
    TSubclassOf<class USessionItem> SessionItemFactory;

    UFUNCTION()
    void FindSession();
    UFUNCTION()
    void OnAddSession(int32 idx, FString info);
    UFUNCTION()
    void OnFindComplete(bool isComplete);
};
