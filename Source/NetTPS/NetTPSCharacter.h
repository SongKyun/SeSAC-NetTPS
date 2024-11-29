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

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MakeCubeAction;
    void MakeCube();

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> cubeFactory;

    UPROPERTY(Replicated) // 서버에서만 변경이 가능하다
    bool canMakeCube = false;

    UFUNCTION(Server, Reliable)
    void ServerRPC_MakeCube();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_MakeCube(FVector pos, FRotator rot);

public:
	ANetTPSCharacter();
	
protected:

	// MainUI 초기화
	void InitMainUIWidget();
    UFUNCTION(Client, Reliable)
    void ClientRPC_Init();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

    UFUNCTION(Server, Reliable)
    void SerVerRPC_TakePistol();
    void TakePistol();

    UFUNCTION()
    void AttachPistol();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_DetachPistol(class APistol* pistol);
    void DetachPistol(class APistol* pistol);

    // 클라에서 서버로 보내는 것이 있어야 한다!!
    UFUNCTION(Server, Reliable)
    void ServerRPC_Fire(bool bHit, FHitResult hitInfo);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_Fire(bool bHit, FHitResult hitInfo);
    void Fire();

    UFUNCTION(Server, Reliable)
    void ServerRPC_Reload();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_Reload();
	void Reload();

public:
	void InitBulletUI();
	void ReloadFinish();
    void BillboardHP();
    void DieProcess();

public:
	// 총이 붙어야 하는 컴포넌트
	UPROPERTY(EditAnywhere)
	USceneComponent* compGun;

	// 총 소유 여부
	bool bHasPistol = false;

	// 내가 잡고 있는 총
    UPROPERTY(ReplicatedUsing = AttachPistol)
	class APistol* ownedPistol = nullptr;

	// 총을 잡을 수 있는 일정범위
	UPROPERTY(EditAnywhere)
	float distanceToGun = 200;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* gunEffect;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* playerMontage;

	// MainUI 위젯 블루프린트 클래스를 담을 변수
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UNetTPSWidget> netTPSWidget;
	
	// 만들어진 MainUI를 담을 변수
	UPROPERTY()
	class UNetTPSWidget* NetTPSUI;

	// 현재 재장전 중인지 여부
	bool IsReloading = false;

	FVector originCamPos;

	// HealBar UI 가지는 컴포넌트
	UPROPERTY(EditAnywhere)
	class UWidgetComponent* compHP;

	void DamageProcess(float damage);

	bool isDead = false;

	void PrintNetLog();


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

    virtual void PossessedBy(AController* NewController) override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};