#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameUI.generated.h"

UCLASS()
class NETTPS_API UGameUI : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY()
    class ANetPlayerState* myPlayerState;
    FTimerHandle playerStateHandle;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Text_PlayerState;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UPlayerStateUI> playerStateUIFactory;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* vBox_PlayerState;

    UPROPERTY()
    TArray<class UPlayerStateUI*> allPlayerState;

    UPROPERTY(meta = (BindWidget))
    class UEditableText* Edit_Chat;

    UPROPERTY(meta = (BindWidget))
    class UScrollBox* Scroll_Chat;

    UPROPERTY(meta = (BindWidget))
    class UBorder* Empty_Border;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UChatItem> chatItemFactory;


    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual void NativeConstruct() override;

    void SetMyPlayerState();

    void AddPlayerStateUI(class APlayerState* ps);

    void AddChat(FString chat);

    UFUNCTION()
    void OnTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod);

    UFUNCTION()
    FEventReply OnPointerEvent(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
};
