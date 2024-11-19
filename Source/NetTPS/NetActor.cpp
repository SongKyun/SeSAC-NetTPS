// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActor.h"
#include "Kismet/GameplayStatics.h"
#include "NetTPSCharacter.h"
#include <Net/UnrealNetwork.h>

// Sets default values
ANetActor::ANetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
    SetRootComponent(compMesh);

    // Repliccate 활성화 : 송신을 가능하게 해주자
    bReplicates = true;
}

// Called when the game starts or when spawned
void ANetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    PrintNetLog();
    FindOwner();
    Rotate();
}

    // 동기화 했을 때 클라이언트마다 호출이 된다
    // 새 클라이언트가 연결되는 시점에 호출이 되어서 로그가 출력된다.
void ANetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Replicate 하고 싶은 변수를 등록해줘야함
    DOREPLIFETIME(ANetActor, rotYaw);

    UE_LOG(LogTemp, Warning, TEXT("111"));
}

void ANetActor::OnRep_RotYaw()
{
    FRotator rot = GetActorRotation();
    rot.Yaw = rotYaw;
    SetActorRotation(rot);
}

void ANetActor::FindOwner()
{
    if (HasAuthority() == false) return;

    TArray<AActor*> allPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetTPSCharacter::StaticClass(), allPlayers);

    AActor* owner = nullptr;
    float closestDistance = searchDistance;
    for (int32 i = 0; i < allPlayers.Num(); i++)
    {
        float dist = FVector::Distance(GetActorLocation(), allPlayers[i]->GetActorLocation());
        if (dist < closestDistance)
        {
            closestDistance = dist;
            owner = allPlayers[i];
        }
    }

    // owner 값을 해당 Actor에 Owner 설정
    if (GetOwner() != owner)
    {
        SetOwner(owner);
    }
}

void ANetActor::Rotate()
{
    // 만약에 서버라면 회전을 실행시킨다
    if (HasAuthority())
    {
        AddActorWorldRotation(FRotator(0, 50 * GetWorld()->DeltaTimeSeconds, 0));
        rotYaw = GetActorRotation().Yaw; // 변수 값을 서버와 동기화 시켜줘야 함
    }
    else
    {
        
    }
}

void ANetActor::PrintNetLog()
{
    // 연결 상태 , null 이면
    FString connStr = GetNetConnection() == nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");

    // Owner(플레이어 컨트롤러를 가지고 있음)
    FString ownerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");

    // 권한
    FString role = UEnum::GetValueAsString<ENetRole>(GetLocalRole());

    FString logStr = FString::Printf(TEXT("Connection : %s\nOwner : %s\nRole : %s"), *connStr, *ownerStr, *role);

    // 드로우 디버그 : 문자열을 화면에 계속 출력
    DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Yellow, 0, true, 1);
}

