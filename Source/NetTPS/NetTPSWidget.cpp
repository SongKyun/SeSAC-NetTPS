// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTPSWidget.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "NetPlayerController.h"

void UNetTPSWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// crosshair image 위젯 가져오자
	imgCrosshair = Cast<UImage>(GetWidgetFromName(TEXT("crosshair")));
	// 초기에 UI 보이지 않게 하기
	ShowCrosshair(false);

	// bulletPanel 위젯 가져오자
	bulletMagazine = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("bulletPanel")));

    // btnRetry 안보이게
    ShowBtnRetry(false);

    btnRetry->OnClicked.AddDynamic(this, &UNetTPSWidget::OnRetry);
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

void UNetTPSWidget::ShowBtnRetry(bool isShow)
{
    if (isShow)
    {
        btnRetry->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        btnRetry->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UNetTPSWidget::AddBullet()
{
	UUserWidget* bullet = CreateWidget(GetWorld(), bulletFactory);
	// 만들어진 bullet을 bulletMagazine에 추가하자
	bulletMagazine->AddChild(bullet);
}

void UNetTPSWidget::PopBullet(int32 index)
{
	bulletMagazine->RemoveChildAt(index);
}

void UNetTPSWidget::PopBulletAll()
{
	int32 remainBullet = bulletMagazine->GetChildrenCount();

	for (int32 i = remainBullet - 1; i >= 0; i--)
	{
		PopBullet(i);
	}
}

void UNetTPSWidget::OnRetry()
{
    // 마우스 커서 안 보이게, 폰이라 컨트롤러가 없어서 
    ANetPlayerController* pc = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
    pc->SetShowMouseCursor(false);
    // 관찰자 모드로 전환
    pc->ServerRPC_ChangeToSpectator();
    // NETTPSWidget UI 삭제하기
    RemoveFromParent();
}
