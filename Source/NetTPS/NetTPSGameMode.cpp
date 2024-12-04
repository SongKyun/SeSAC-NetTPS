#include "NetTPSGameMode.h"
#include "NetTPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

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

    // Spawn 자리 갯수
    int32 spawnPosCount = 4;
    
    // Spawn 각도
    float spawnDegree = 360.0f / spawnPosCount;
    
    // PlayerStart 찾아오자
    AActor* playerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());

    // 각도를 radian 값으로 변경
    float radian = FMath::DegreesToRadians(posIdx * spawnDegree);

    // 삼각 함수
    FVector pos;
    FVector dir = FVector(
        FMath::Sin(posIdx * spawnDegree),
        FMath::Cos(posIdx * spawnDegree),
        0
    );
    pos = playerStart->GetActorLocation() + dir * 200;
    player->SetActorLocation(pos);
    posIdx = (posIdx + 1) % spawnPosCount;

    // 액터를 회전시켜서 회전 액터의 앞방향을 이용
    /*playerStart->SetActorRotation(FRotator(0, posIdx * spawnDegree, 0));
    FVector pos = playerStart->GetActorLocation() + playerStart->GetActorForwardVector() * 200;
    player->SetActorLocation(pos);
    posIdx = (posIdx + 1) % spawnPosCount;*/

    // spawnPosCount 반복해서 playerStart를 spawnDegree 만큼 회전
    //for (int32 i = 0; i < spawnPosCount; i++)
    //{
    //    playerStart->SetActorRotation(FRotator(0, i * spawnDegree, 0));
    //    FVector pos = playerStart->GetActorLocation() + playerStart->GetActorForwardVector() * 200;
    //}
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
