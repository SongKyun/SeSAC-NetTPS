// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSGameMode.h"
#include "NetTPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetTPSGameMode::ANetTPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ANetTPSGameMode::AddPlayer(ANetTPSCharacter* player)
{
    allPlayers.Add(player);
}

void ANetTPSGameMode::ChangeTurn()
{
    // 현재 turnIdx의 Player 에게 큐브 생성 못하게 하자 : 증가시키기 전에 변수 이용해 false로 하고서 진행
    allPlayers[turnIdx]->canMakeCube = false;

    // 턴을 증가시키자
    turnIdx++;

    // 만약에 turnIdx 가 allPlayers 의 갯수가 같거나 크다면
    if (turnIdx >= allPlayers.Num())
    {
        // 턴을 처음으로 돌리자
        turnIdx = 0;
    }

    // turnidx = (turnIdx + 1) % allPlayers.Num(); 이렇게 1줄로도 위의 로직을 구현할 수 있다

    // turnIdx 에 있는 Player가 큐브를 만들 수 있게 하자
    allPlayers[turnIdx]->canMakeCube = true;
}
