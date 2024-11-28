#include "LobbyGameMode.h"
#include "LobbyUI.h"

void ALobbyGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Lobby UI 화면에 붙히자
    lobbyUI = CreateWidget<ULobbyUI>(GetWorld(), lobbyUIFactory);
    lobbyUI->AddToViewport();

    // 마우스 활성화 하자
    APlayerController* pc = GetWorld()->GetFirstPlayerController();
    pc->SetShowMouseCursor(true);
    // Input Mode를 UI만 동작하게 하자
}