// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetTPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetTPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* TakeAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ReloadAction;

public:
	ANetTPSCharacter();
	
protected:

	// MainUI �ʱ�ȭ
	void InitMainUIWidget();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void TakePistol();
	void AttachPistol(class APistol* pistol);
	void DetachPistol();

	void Fire();
	void Reload();

public:
	void ReloadFinish();
	void InitBulletUI();
	
public:
	// ���� �پ�� �ϴ� ������Ʈ
	UPROPERTY(EditAnywhere)
	USceneComponent* compGun;

	// �� ���� ����
	bool bHasPistol = false;

	// ���� ��� �ִ� ��
	UPROPERTY()
	class APistol* ownedPistol = nullptr;

	// ���� ���� �� �ִ� ��������
	UPROPERTY(EditAnywhere)
	float distanceToGun = 200;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* gunEffect;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* playerMontage;

	// MainUI ���� �������Ʈ Ŭ������ ���� ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UNetTPSWidget> netTPSWidget;
	
	// ������� MainUI�� ���� ����
	UPROPERTY()
	class UNetTPSWidget* NetTPSUI;

	// ���� ������ ������ ����
	bool IsReloading = false;

	FVector originCamPos;

	// HealBar UI ������ ������Ʈ
	UPROPERTY(EditAnywhere)
	class UWidgetComponent* compHP;
	// �ִ� HP
	float maxHP = 100;
	// ���� HP
	float currHP;

	void DamageProcess(float damage);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};