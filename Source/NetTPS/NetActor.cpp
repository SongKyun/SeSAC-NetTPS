// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActor.h"
#include "Kismet/GameplayStatics.h"
#include "NetTPSCharacter.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/KismetMathLibrary.h>

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
	
    // 매터리얼 복제
    mat = compMesh->CreateDynamicMaterialInstance(0);

    // 1초마다 ChangeScale 함수 호출하는 타이머 등록
    FTimerHandle handle;
    GetWorldTimerManager().SetTimer(handle, this, &ANetActor::ChangeScale, 1.0f, true);

    if (HasAuthority())
    {
        FTimerHandle handleLocation;
        GetWorldTimerManager().SetTimer(handleLocation, this, &ANetActor::ChangeLocation, 1.0f, true);
    }
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    PrintNetLog();
    FindOwner();
    Rotate();
    ChangeColor();
}

    // 동기화 했을 때 클라이언트마다 호출이 된다
    // 새 클라이언트가 연결되는 시점에 호출이 되어서 로그가 출력된다.
void ANetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Replicate 하고 싶은 변수를 등록해줘야함
    DOREPLIFETIME(ANetActor, rotYaw);

    DOREPLIFETIME(ANetActor, matColor); // ChangeColor() 쪽 변수

    //UE_LOG(LogTemp, Warning, TEXT("111"));
}

void ANetActor::OnRep_RotYaw()
{
    FRotator rot = GetActorRotation();
    rot.Yaw = rotYaw;
    SetActorRotation(rot);
}

void ANetActor::OnRep_ChangeColor() // 클라에서만 호출되어서 서버에서는 호출되지 않는다
{
    // 해당 색을 매터리얼에 설정하자
    mat->SetVectorParameterValue(TEXT("FloorColor"), matColor);
}

void ANetActor::ChangeColor()
{
    // 만약에 서버라면
    if (HasAuthority())
    {
        // 시간을 흐르게 하자
        currTime += GetWorld()->DeltaTimeSeconds;
        // 만약에 현재 시간이 색상변경시간보다 커지면
        if (currTime > changeTime)
        {
            // 랜덤한 색상 뽑아내고
            matColor = FLinearColor::MakeRandomColor(); // 멤버 변수로 바꿔준다
            // 해당 색을 매터리얼에 설정하자
            mat->SetVectorParameterValue(TEXT("FloorColor"), matColor);

            currTime = 0;
        }
    }
    else
    {
        mat->SetVectorParameterValue(TEXT("FloorColor"), matColor);
    }
}

void ANetActor::ChangeScale()
{
    if (GetOwner() == GetWorld()->GetFirstPlayerController()->GetPawn())
    {
        // 서버에게 크기 변경 요청
        ServerRPC_ChangeScale();
    }
}

void ANetActor::ServerRPC_ChangeScale_Implementation() // 무조건 서버에서만 호출된다
{
    UE_LOG(LogTemp, Warning, TEXT("ServerRPC_ChangeScale_Implementation"));
    float rand = FMath::RandRange(0.5f, 2.0f);

    // 클라이언트 한테 rand 만큼 스케일 크기 변경되게 알려주자
    //ClientRPC_ChangeScale(FVector(rand));

    MulticastRPC_ChangeScale(FVector(rand)); // 오너 설정을 자동으로 해서 가까이 가지 않아도 크기가 변경된다
}

void ANetActor::ClientRPC_ChangeScale_Implementation(FVector scale)
{
    SetActorScale3D(scale);
}

void ANetActor::MulticastRPC_ChangeScale_Implementation(FVector scale)
{
    SetActorScale3D(scale);
}

void ANetActor::ChangeLocation()
{
    // 랜덤한 위치를 뽑아냄
    FVector rand = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), FVector(20));
    // rand 모든 클라이언트에 알려주자
    MulticastRPC_ChangeLocation(rand);
}

void ANetActor::MulticastRPC_ChangeLocation_Implementation(FVector NewLocation)
{
    SetActorLocation(NewLocation);
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

