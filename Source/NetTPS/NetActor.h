// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetActor.generated.h"

UCLASS()
class NETTPS_API ANetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    UPROPERTY(EditAnywhere)
    class UStaticMeshComponent* compMesh;

    // 발견 범위
    float searchDistance = 200;

    // yaw 회전값
    //UPROPERTY(Replicated) // 변수 동기화 하겠다
    //float rotYaw = 0;

    UPROPERTY(ReplicatedUsing = OnRep_RotYaw) // 추가적으로 함수까지 호출해준다
    float rotYaw = 0;
    UFUNCTION()
    void OnRep_RotYaw();

    // 매터리얼 색상 변경
    UPROPERTY()
    class UMaterialInstanceDynamic* mat;

    float currTime = 0;
    float changeTime = 2.0f;

    UPROPERTY(ReplicatedUsing = OnRep_ChangeColor)
    FLinearColor matColor;
    UFUNCTION()
    void OnRep_ChangeColor();
    void ChangeColor();

    // RPC 이용해서 크기변경
    void ChangeScale();

    // 서버 RPC
    UFUNCTION(Server, Reliable) // Server(서버), Reliable(신뢰)
    void ServerRPC_ChangeScale();

    // 클라이언트 RPC
    UFUNCTION(Client, Reliable)
    void ClientRPC_ChangeScale(FVector scale);

    // Multicast RPC 모두 동기화를 위해
    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_ChangeScale(FVector scale);

    // RPC 이용해서 위치를 바꿔보자
    void ChangeLocation();
    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_ChangeLocation(FVector NewLocation);

    void PrintNetLog();
    void FindOwner();

    void Rotate();
};
