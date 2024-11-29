#include "NetGameState.h"
#include "GameUI.h"

void ANetGameState::BeginPlay()
{
    Super::BeginPlay();
}

UGameUI* ANetGameState::GetGameUI()
{
    if(gameUI == nullptr)
    { 
        // GameUI 생성
        gameUI = CreateWidget<UGameUI>(GetWorld(), gameUIFactory);
        gameUI->AddToViewport();
    }

    return gameUI;
}