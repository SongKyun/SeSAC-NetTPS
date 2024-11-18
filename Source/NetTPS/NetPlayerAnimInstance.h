// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UNetPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	// ���� ��� �ִ��� ���� üũ ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasPistol = false;

	// dirH, dirV
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float dirH = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float dirV = 0;

	// ���� üũ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDead = false;

	UPROPERTY()
	class ANetTPSCharacter* player;



public:
	// Reload �ִϸ��̼� ������ �κп� ȣ��Ǵ� �Լ�
	UFUNCTION()
	void AnimNotify_OnReloadFinish();
};
