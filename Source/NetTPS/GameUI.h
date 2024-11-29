#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameUI.generated.h"

UCLASS()
class NETTPS_API UGameUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Text_PlayerState;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UPlayerStateUI> playerStateUIFactory;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* vBox_PlayerState;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void AddPlayerStateUI(class APlayerState* ps);
};
