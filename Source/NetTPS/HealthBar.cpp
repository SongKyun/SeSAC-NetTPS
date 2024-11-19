#include "HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	currHP = maxHP;
}

float UHealthBar::UpdateHPBar(float damage)
{
	// ���� HP�� damage ��ŭ �ٿ�����.
	currHP -= damage;

	// ���� HP �� 0 ~ 1 ������ ������ �ٲ���
	float percent = currHP / maxHP;

	// ProgressBar ����
	HPBar->SetPercent(percent);

	// ���� HP �� ��ȯ����
	return currHP;
}
