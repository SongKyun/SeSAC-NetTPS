// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "NetTPSWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetTPSCharacter

ANetTPSCharacter::ANetTPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0, 0, 60));
	CameraBoom->TargetArmLength = 300; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Gun Component
	compGun = CreateDefaultSubobject<USceneComponent>(TEXT("GUN"));
	compGun->SetupAttachment(GetMesh(), TEXT("gunPosition"));
	compGun->SetRelativeLocation(FVector(-7.144f, 3.68f, 4.136f));
	compGun->SetRelativeRotation(FRotator(3.4f, 75.699f, 6.642f));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ANetTPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	InitMainUIWidget();

	// originCamPos를 초기의 CameraBoom 값으로 설정
	originCamPos = CameraBoom->GetRelativeLocation();
}

void ANetTPSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// CameraBoom이 originCamPos를 향해서 계속 움직이자
	FVector pos = FMath::Lerp(CameraBoom->GetRelativeLocation(), originCamPos, DeltaSeconds * 10);
	CameraBoom->SetRelativeLocation(pos);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetTPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Look);

		// F 키 눌렀을 때 호출되는 함수 등록
		EnhancedInputComponent->BindAction(TakeAction, ETriggerEvent::Started, this, &ANetTPSCharacter::TakePistol);
		// 마우스 왼쪽 버튼 호출 함수 등록
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ANetTPSCharacter::Fire);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ANetTPSCharacter::Reload);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTPSCharacter::InitMainUIWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("UI"));

	NetTPSUI = Cast<UNetTPSWidget>(CreateWidget(GetWorld(), netTPSWidget));
	NetTPSUI->AddToViewport();

	// 총알 UI 생성
	currBulletCount = maxBulletCount;
	for (int i = 0; i < currBulletCount; i++)
	{
		NetTPSUI->AddBullet();
	}
}

void ANetTPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetTPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANetTPSCharacter::TakePistol()
{
	// 총을 소유하고 있지 않다면 일정범위 안에 있는 총을 잡는다.
	// 1. 총을 잡고 있지 않다면
	if (bHasPistol == false)
	{
		// 2. 월드에 있는 총을 모두 찾는다.
		TArray<AActor*> allActors;
		TArray<AActor*> pistolActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActors);
		for (int i = 0; i < allActors.Num(); i++)
		{
			if (allActors[i]->GetActorLabel().Contains(TEXT("BP_Pistol")))
			{
				pistolActors.Add(allActors[i]);
			}
		}

		// 나와 총의 최단거리
		float closestDist = std::numeric_limits<float>::max();
		AActor* closestPistol = nullptr;

		for (AActor* pistol : pistolActors)
		{
			// 만약에 pistol 의 소유자가 있다면 - 거리를 구할 필요가 없다
			if (pistol->GetOwner() == nullptr)
			{
				// 3. 총과의 거리를 구하자.
				float dist = FVector::Distance(pistol->GetActorLocation(), GetActorLocation());
				// 4. 만약에 거리가 일정범위 안에 있다면
				if (dist < distanceToGun)
				{
					// closesDist 값보다 dist 값이 크다면 ( 더 가깝다는 의미 )
					if (closestDist > dist)
					{
						// 최단거리 갱신
						closestDist = dist;
						closestPistol = pistol;
					}
				}
			}
		}
		AttachPistol(closestPistol);
	}
	else // 총을 잡고 있다면
	{
		// 총을 놓자 , 순서를 염두
		DetachPistol();
	}
}

void ANetTPSCharacter::AttachPistol(AActor* pistol)
{
	// 총을 Mesh 의 손에 붙히자
	if (pistol == nullptr) return; // null 체크
	pistol->SetOwner(this);
	bHasPistol = true;
	ownedPistol = pistol;

	// pistol이 가지고 있는 StaticMesh 컴포넌트 가져오자
	UStaticMeshComponent* comp = pistol->GetComponentByClass<UStaticMeshComponent>();
	// 가져온 컴포넌트를 이용해서 SimulatePhysics 비활성화
	comp->SetSimulatePhysics(false);
	// Mesh - gunPosition 소켓에 붙히자
	pistol->AttachToComponent(compGun, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// 총 들었을 때 카메라 캐릭터 회전 기능 변경 (카메라에 의해서 변경되도록)
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	CameraBoom->TargetArmLength = 150;
	originCamPos = FVector(0, 40, 60);

	// UI 보이게 하기
	NetTPSUI->ShowCrosshair(true);
}

void ANetTPSCharacter::DetachPistol()
{
	UStaticMeshComponent* comp = ownedPistol->GetComponentByClass<UStaticMeshComponent>();
	comp->SetSimulatePhysics(true);
	ownedPistol->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	// 총 놨을 때 카메라 캐릭터 회전 기능 변경 (카메라와 독립)
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	CameraBoom->TargetArmLength = 300;
	originCamPos = FVector(0, 0, 60);

	// UI 보이지 않게 하기
	NetTPSUI->ShowCrosshair(false);

	bHasPistol = false;
	ownedPistol->SetOwner(nullptr);
	ownedPistol = nullptr;
}

void ANetTPSCharacter::Fire()
{
	// 만약에 총을 들고 있지 않다면 함수를 나가자
	if (bHasPistol == false) return;
	
	// 현재 총알 갯수가 0보다 작거나 같으면 함수를 나간다
	if (currBulletCount <= 0) return;

	// 재장전 중이면 나가자
	if (IsReloading) return;

	// LineTrace 로 부딪힌 위치 찾기
	FVector startPos = FollowCamera->GetComponentLocation(); // 월드 좌표 Location
	FVector endPos = startPos + FollowCamera->GetForwardVector() * 100000;

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	FHitResult hitInfo;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECollisionChannel::ECC_Visibility, params);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s, %s"),
			*hitInfo.GetActor()->GetActorLabel(),
			*hitInfo.GetActor()->GetName());

		// 맞은 위치에  파티클로 표시 하자.
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), gunEffect, hitInfo.Location, FRotator(), true);
	}

	// 총 쏘는 애니메이션을 실행하자
	PlayAnimMontage(playerMontage, 2, TEXT("Fire"));

	// 총알 제거
	currBulletCount--;
	NetTPSUI->PopBullet(currBulletCount);
}

void ANetTPSCharacter::Reload()
{
	// 총을 가지고 있지 않고
	if (bHasPistol == false) return;

	// 현재 재장전 중이면 함수를 나가자
	if (IsReloading) return;

	IsReloading = true;

	// 장전 애니메이션 실행
	PlayAnimMontage(playerMontage, 1, TEXT("Reload"));
}

void ANetTPSCharacter::ReloadFinish()
{
	IsReloading = false;

	// 빈 총알 UI 를 채우자.
	// 채워야 하는 총알 갯수 계산
	int32 addBulletCount = maxBulletCount - currBulletCount;

	// 현재 총알 갯수를 최대 총알 갯수로 설정
	currBulletCount = maxBulletCount;

	for (int i = 0; i < addBulletCount; i++)
	{
		NetTPSUI->AddBullet();
	}
}
