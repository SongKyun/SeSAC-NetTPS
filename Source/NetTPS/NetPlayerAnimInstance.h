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

	UPROPERTY()
	class ANetTPSCharacter* player;

};