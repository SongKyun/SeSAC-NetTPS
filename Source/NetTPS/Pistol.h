// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pistol.generated.h"

UCLASS()
class NETTPS_API APistol : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APistol();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �ִ� �Ѿ� ����
	UPROPERTY(EditAnywhere)
	float maxBulletCount = 10;

	// ���� �Ѿ� ����
	float currBulletCount = 0;

	// ���� ������
	float weaponDamage = 20;

	bool IsMaxBulletCount();
	void Reload();
};
