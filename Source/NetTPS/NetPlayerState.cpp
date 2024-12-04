#include "NetPlayerState.h"
#include "NetGameState.h"
#include "GameUI.h"

ANetPlayerState::ANetPlayerState(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    NetUpdateFrequency = 100;
}

void ANetPlayerState::OnRep_Score()
{
    Super::OnRep_Score();

    // 점수 갱신을 GameUI가 가지고 있는 PlayerStateUI에게 알려주자.
    onUpdateScore.ExecuteIfBound();
}

void ANetPlayerState::ServerRPC_SendChat_Implementation(const FString& chat)
{
    MulticastRPC_SendChat(chat);
}

void ANetPlayerState::MulticastRPC_SendChat_Implementation(const FString& chat)
{
    ANetGameState* gameState = GetWorld()->GetGameState<ANetGameState>();
    gameState->gameUI->AddChat(chat);
}

void ANetPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (HasAuthority())
    {
        Cast<ANetGameState>(GetWorld()->GetGameState())->LeavePlayer();
    }
}