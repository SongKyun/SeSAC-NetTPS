#include "NetPlayerAnimInstance.h"
#include "NetTPSCharacter.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// ���� �پ��ִ� Pawn ��������
	player = Cast<ANetTPSCharacter>(TryGetPawnOwner());
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (player)
	{
		// ��, �� ���⿡ ���� dirV �� ������
		dirV = FVector::DotProduct(player->GetActorForwardVector(), player->GetVelocity());
		// ��, �� ���⿡ ���� dirH �� ������
		dirH = FVector::DotProduct(player->GetActorRightVector(), player->GetVelocity());
	}
}