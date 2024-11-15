// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTPSWidget.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"

void UNetTPSWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// crosshair image ���� ��������
	imgCrosshair = Cast<UImage>(GetWidgetFromName(TEXT("crosshair")));
	// �ʱ⿡ UI ������ �ʰ� �ϱ�
	ShowCrosshair(false);

	// bulletPanel ���� ��������
	bulletMagazine = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("bulletPanel")));
}

void UNetTPSWidget::ShowCrosshair(bool isShow)
{
	if (isShow)
	{
		imgCrosshair->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		imgCrosshair->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UNetTPSWidget::AddBullet()
{
	UUserWidget* bullet = CreateWidget(GetWorld(), bulletFactory);
	// ������� bullet�� bulletMagazine�� �߰�����
	bulletMagazine->AddChild(bullet);
}

void UNetTPSWidget::PopBullet(int32 index)
{
	bulletMagazine->RemoveChildAt(index);
}
