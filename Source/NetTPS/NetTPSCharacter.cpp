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
#include "Pistol.h"
#include "Components/WidgetComponent.h"
#include "HealthBar.h"
#include <Kismet/KismetMathLibrary.h>
#include "NetTPSGameMode.h"
#include <Net/UnrealNetwork.h>
#include "GameFramework/PlayerState.h"
#include "NetGameState.h"
#include "GameUI.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

void ANetTPSCharacter::MakeCube()
{
    // 내 차례가 아니면 함수 나가자
    if (canMakeCube == false) return;

    ServerRPC_MakeCube();
}

void ANetTPSCharacter::ServerRPC_MakeCube_Implementation()
{
    // 턴 넘기자
    // 게임 모드 가져오자 (ANetTPSGameMode 형변환)
    ANetTPSGameMode* gm = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
    gm->ChangeTurn();

    FVector pos = GetActorLocation() + GetActorForwardVector() * 100;
    MulticastRPC_MakeCube(pos, GetActorRotation());
}

void ANetTPSCharacter::MulticastRPC_MakeCube_Implementation(FVector pos, FRotator rot)
{
    // 큐브 생성하자
    AActor* cube = GetWorld()->SpawnActor<AActor>(cubeFactory);
    // 큐브의 위치를 나의 앞 방향으로 100만큼 떨어진 위치에 스폰
    //FVector pos = GetActorLocation() + GetActorForwardVector() * 100;
    cube->SetActorLocation(pos);
    //UE_LOG(LogTemp, Warning, TEXT("%s"), *pos.ToString());
    // 큐브의 회전값을 나의 회전값으로 설정
    cube->SetActorRotation(rot);
}

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

	// HP Widget Component 위젯 컴포넌트
	compHP = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP"));
	compHP->SetupAttachment(RootComponent);
}

void ANetTPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

    FString isServer = HasAuthority() ? TEXT("서버") : TEXT("클라");
    UE_LOG(LogTemp, Warning, TEXT("%s - BeginPlay : %s"), *isServer, *GetActorNameOrLabel());

	// originCamPos를 초기의 CameraBoom 값으로 설정
	originCamPos = CameraBoom->GetRelativeLocation();

    // 서버라면
    if (HasAuthority()) // 이거로 서버로 막아줌
    {
        ANetTPSGameMode* gm = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
        // 나를 추가 시키자
        if (gm) // 널 체크를 하지 않으면 클라에서 추가를 하기 때문에? 클라는 게임모드가 없어서 gm 이 널이기에 널 상태에서 하면 크래쉬
        {
            gm->AddPlayer(this);
        }
        // 서버이면서 내 캐릭터라면
        if (IsLocallyControlled())
        {
            canMakeCube = true;
        }
    }
}

void ANetTPSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// CameraBoom이 originCamPos를 향해서 계속 움직이자
	FVector pos = FMath::Lerp(CameraBoom->GetRelativeLocation(), originCamPos, DeltaSeconds * 10);
	CameraBoom->SetRelativeLocation(pos);

	//PrintNetLog();
    BillboardHP();

    if (IsLocallyControlled())
    {
        // 코드로 키보드 Press, Release 이벤트 받아오기
        APlayerController* pc = GetWorld()->GetFirstPlayerController();
        if (pc->WasInputKeyJustPressed(EKeys::F10))
        {
            UE_LOG(LogTemp, Warning, TEXT("F10 키 눌림"));
        }
        if (pc->WasInputKeyJustReleased(EKeys::F10))
        {
            UE_LOG(LogTemp, Warning, TEXT("F10 키 뗌"));
        }
        if (pc->IsInputKeyDown(EKeys::F10))
        {
            UE_LOG(LogTemp, Warning, TEXT("F10 눌리고 있음"));
        }
    }
}

void ANetTPSCharacter::DamageProcess(float damage)
{
	// HPBar를 갱신
    UHealthBar* hpBar = nullptr;
    // 내것이라면 HealthBar를 갱신
    if (IsLocallyControlled())
    {
        hpBar = NetTPSUI->HealthBar;
    }
    else
    {
        hpBar = Cast<UHealthBar>(compHP->GetWidget());
    }

	float currHP = hpBar->UpdateHPBar(damage);
	if (currHP <= 0)
	{
		// 죽음 처리
		isDead = true;

        // 움직이지 못하게 하자
        GetCharacterMovement()->DisableMovement();

        // 충돌 안되게 하자
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        // 만약에 총들고 있으면 내 캐릭터라면 총을 드랍하자
        if (bHasPistol && IsLocallyControlled())
        {
            TakePistol();
        }
	}
}

void ANetTPSCharacter::PrintNetLog()
{
	// 연결 상태 , null 이면
	FString connStr = GetNetConnection() == nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");

	// Owner(플레이어 컨트롤러를 가지고 있음)
	FString ownerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");

	// 권한
	FString role = UEnum::GetValueAsString<ENetRole>(GetLocalRole());

	// 내것인지
	FString isMine = IsLocallyControlled() ? TEXT("내 것") : TEXT("남의 것");

	FString logStr = FString::Printf(TEXT("Connection : %s\nOwner : %s\nRole : %s\nmine : %s"), *connStr, *ownerStr, *role, *isMine);

	// 드로우 디버그 : 문자열을 화면에 계속 출력
	DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Yellow, 0, true, 1);

	// 컨트롤 체크하는 언리얼 내부 구현 함수 HasAuthority();
}

void ANetTPSCharacter::SerVerRPC_TakePistol_Implementation()
{
    // 총을 소유하고 있지 않다면 일정범위 안에 있는 총을 잡는다.
    // 1. 총을 잡고 있지 않다면
    if (bHasPistol == false)
    {
        // 2. 월드에 있는 총을 모두 찾는다.
        TArray<AActor*> allActors;
        TArray<APistol*> pistolActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APistol::StaticClass(), allActors);
        for (int i = 0; i < allActors.Num(); i++)
        {
            pistolActors.Add(Cast<APistol>(allActors[i]));
        }

        // 나와 총의 최단거리
        float closestDist = std::numeric_limits<float>::max();
        APistol* closestPistol = nullptr;

        for (APistol* pistol : pistolActors)
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

        if (closestPistol)
        {
            ownedPistol = closestPistol;
            // Owner 설정
            closestPistol->SetOwner(this);
            AttachPistol();
        }
    }
    else // 총을 잡고 있다면
    {
        APistol* pistol = ownedPistol;
        // Owner 설정 ownedPistol 내가 잡고 있는 총
        ownedPistol->SetOwner(nullptr);
        ownedPistol = nullptr;
        // 총을 놓자 , 순서를 염두
        MulticastRPC_DetachPistol(pistol);
    }
}

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

		//EnhancedInputComponent->BindAction(MakeCubeAction, ETriggerEvent::Started, this, &ANetTPSCharacter::MakeCube);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANetTPSCharacter, canMakeCube); // 변수 동기화를 위해서 설정
    DOREPLIFETIME(ANetTPSCharacter, ownedPistol);
}

void ANetTPSCharacter::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
    Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

    // GameState 가져오고
    ANetGameState* gameState = Cast<ANetGameState>(GetWorld()->GetGameState());
    // GameUI 가져와서 PlayerStateUI 하나 만들어주세요
    gameState->GetGameUI()->AddPlayerStateUI(NewPlayerState);
}

void ANetTPSCharacter::InitMainUIWidget()
{
    // 만약에 내 캐릭터가 아니라면 함수를 나가자
    if (IsLocallyControlled() == false) return;

	NetTPSUI = Cast<UNetTPSWidget>(CreateWidget(GetWorld(), netTPSWidget));
	NetTPSUI->AddToViewport();

    // 내 HPBar 컴포넌트 안 보이게
    compHP->SetVisibility(false);
}

void ANetTPSCharacter::ClientRPC_Init_Implementation()
{
    InitMainUIWidget();
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
    SerVerRPC_TakePistol();
}

void ANetTPSCharacter::AttachPistol()
{	
    if (ownedPistol == nullptr) return;
    bHasPistol = true;

	// pistol이 가지고 있는 StaticMesh 컴포넌트 가져오자
	UStaticMeshComponent* comp = ownedPistol->GetComponentByClass<UStaticMeshComponent>();

	// 가져온 컴포넌트를 이용해서 SimulatePhysics 비활성화
	comp->SetSimulatePhysics(false);

	// Mesh - gunPosition 소켓에 붙히자
	ownedPistol->AttachToComponent(compGun, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    if (IsLocallyControlled())
    {
        // 총 들었을 때 카메라 캐릭터 회전 기능 변경 (카메라에 의해서 변경되도록)
        CameraBoom->TargetArmLength = 150;
        originCamPos = FVector(0, 40, 60);

        // ownedPistol의 현재 총알 갯수만큼 총알 UI를 채우자
        InitBulletUI();
    }
}

void ANetTPSCharacter::MulticastRPC_DetachPistol_Implementation(APistol* pistol)
{
    DetachPistol(pistol);
}

void ANetTPSCharacter::DetachPistol(APistol* pistol)
{
	if (IsReloading) return;

	UStaticMeshComponent* comp = pistol->GetComponentByClass<UStaticMeshComponent>();
	comp->SetSimulatePhysics(true);
    pistol->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    if (IsLocallyControlled())
    {
        // 총 놨을 때 카메라 캐릭터 회전 기능 변경 (카메라와 독립)
        CameraBoom->TargetArmLength = 300;
        originCamPos = FVector(0, 0, 60);

        // UI 보이지 않게 하기
        NetTPSUI->ShowCrosshair(false);

        // 총알 UI 지우자
        NetTPSUI->PopBulletAll();
    }

	bHasPistol = false;
	//ownedPistol->SetOwner(nullptr);
	ownedPistol = nullptr;
}

void ANetTPSCharacter::ServerRPC_Fire_Implementation(bool bHit, FHitResult hitInfo)
{
    // 만약에 맞은 Actor 가 Player라면
    ANetTPSCharacter* player = Cast<ANetTPSCharacter>(hitInfo.GetActor());
    if (player)
    {
        APlayerState* ps = GetPlayerState();
        ps->SetScore(ps->GetScore() + 1);
        ps->OnRep_Score();
    }

    // 모든 클라에게 전달
    MulticastRPC_Fire(bHit, hitInfo);
}

void ANetTPSCharacter::MulticastRPC_Fire_Implementation(bool bHit, FHitResult hitInfo)
{

    if (bHit) // 맞은 경우 안의 로직이 실행됨
    {
        // 맞은 위치에  파티클로 표시 하자.
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), gunEffect, hitInfo.Location, FRotator(), true);

        // 만약에 맞은 Actor가 Player 라면
        ANetTPSCharacter* player = Cast<ANetTPSCharacter>(hitInfo.GetActor());
        if (player)
        {
            // 벽을 만나면 널체크를 ..
            // 해당 Player가 가지고 있는 DamageProcess 함수 실행
            player->DamageProcess(ownedPistol->weaponDamage);
        }
    }

    // 맞지 않은 경우에도 실행됨
    // 총알 제거
    ownedPistol->currBulletCount--;
    if (IsLocallyControlled())
    {
        NetTPSUI->PopBullet(ownedPistol->currBulletCount);
    }

    // 총 쏘는 애니메이션을 실행하자
    PlayAnimMontage(playerMontage, 2, TEXT("Fire"));
}

void ANetTPSCharacter::Fire()
{
	// 만약에 총을 들고 있지 않다면 함수를 나가자
	if (bHasPistol == false) return;
	
	// 현재 총알 갯수가 0보다 작거나 같으면 함수를 나간다
	if (ownedPistol->currBulletCount <= 0) return;

	// 재장전 중이면 나가자
	if (IsReloading) return;

	// LineTrace 로 부딪힌 위치 찾기
	FVector startPos = FollowCamera->GetComponentLocation(); // 월드 좌표 Location
	FVector endPos = startPos + FollowCamera->GetForwardVector() * 100000;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	FHitResult hitInfo;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECollisionChannel::ECC_Visibility, params);

    // 서버에서 파티클 나오게 요청!
    ServerRPC_Fire(bHit, hitInfo);
}

void ANetTPSCharacter::ServerRPC_Reload_Implementation()
{
    // 총을 가지고 있지 않고
    if (bHasPistol == false) return;
    // 현재 총알 갯수가 최대 총알 갯수와 같으면 함수를 나가자.
    if (ownedPistol->IsMaxBulletCount()) return;
    // 현재 재장전 중이면 함수를 나가자.
    if (IsReloading) return;

    MulticastRPC_Reload();
}

void ANetTPSCharacter::MulticastRPC_Reload_Implementation()
{

    IsReloading = true;

    // 장전 애니메이션 실행
    PlayAnimMontage(playerMontage, 1, TEXT("Reload"));
}

void ANetTPSCharacter::Reload()
{
	// 총을 가지고 있지 않고
	if (bHasPistol == false) return;

	// 현재 재장전 중이면 함수를 나가자
	if (ownedPistol->IsMaxBulletCount()) return;
    // 현재 재장전 중이면 함수를 나가자
	IsReloading = true;

	// 장전 애니메이션 실행
	PlayAnimMontage(playerMontage, 1, TEXT("Reload"));
}

void ANetTPSCharacter::ReloadFinish()
{
	IsReloading = false;

	// 현재 총알 갯수를 최대 총알 갯수로 설정
	ownedPistol->Reload();

	InitBulletUI();
}

void ANetTPSCharacter::InitBulletUI()
{
    // 만약 내 캐릭터가 아니라면 함수 나가자
    if (IsLocallyControlled() == false) return;

    // UI 보이게 하기
    NetTPSUI->ShowCrosshair(true);

	// 총알 UI 다 지우자
	NetTPSUI->PopBulletAll();

	// addBulletCount만큼 총알 UI 채우자
	for (int i = 0; i < ownedPistol->currBulletCount; i++)
	{
		NetTPSUI->AddBullet();
	}
}

void ANetTPSCharacter::BillboardHP()
{
    // 왜 플레이어 카메라를 안 쓰고 월드에서 카메라를 찾아오는가?
    // 내가 컨트롤하는 쓰고 있는 카메라를 기준으로 바라보게 해야하기 때문이다.

        // 카메라 찾자
        AActor* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        // 카메라 앞방향
        FVector forward = -cam->GetActorForwardVector();
        // 카메라 윗방향
        FVector up = cam->GetActorUpVector();

        // 위 두 방향을 이용해서 HP 바의 회전 값을 구하자 XZ 앞위 방향을 이용
        FRotator rot = UKismetMathLibrary::MakeRotFromXZ(forward, up);

        compHP->SetWorldRotation(rot);
}

void ANetTPSCharacter::DieProcess()
{
    if (IsLocallyControlled() == false) return;

    // 회색 화면 처리
    FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 1);

    // 마우스 커서 보이게 하자
    APlayerController* pc = Cast<APlayerController>(Controller);
    pc->SetShowMouseCursor(true);

    // 다시하기 버튼 보이게
    NetTPSUI->ShowBtnRetry(true);
}

void ANetTPSCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    UE_LOG(LogTemp, Warning, TEXT("PossessedBy : %s"), *GetActorNameOrLabel());

    ClientRPC_Init();
}
