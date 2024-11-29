#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NetGameState.generated.h"

UCLASS()
class NETTPS_API ANetGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
    virtual void BeginPlay() override;

    class UGameUI* GetGameUI();

public:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UGameUI> gameUIFactory;

    UPROPERTY()
    class UGameUI* gameUI;
};