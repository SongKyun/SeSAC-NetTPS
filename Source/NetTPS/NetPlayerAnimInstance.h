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
	// 총을 들고 있는지 여부 체크 변수 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasPistol = false;

	// dirH, dirV
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float dirH = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float dirV = 0;

	// 죽음 체크
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDead = false;

	UPROPERTY()
	class ANetTPSCharacter* player;



public:
	// Reload 애니메이션 끝나는 부분에 호출되는 함수
	UFUNCTION()
	void AnimNotify_OnReloadFinish();

    // 죽음 애니메이션 끝날 때 호출되는 함수
    UFUNCTION()
    void AnimNotify_OnDie();
};
