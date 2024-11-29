#include "NetPlayerState.h"
#include "NetPlayerState.h"

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
