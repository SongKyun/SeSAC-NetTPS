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

    void FindOwner();
    void Rotate();
    void PrintNetLog();
};
