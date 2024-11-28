#include "LobbyUI.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components//ScrollBox.h"
#include "NetGameInstance.h"
#include "SessionItem.h"

void ULobbyUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 게임 인스턴스 찾아오자
    gi = Cast<UNetGameInstance>(GetGameInstance());

    Btn_Create->OnClicked.AddDynamic(this, &ULobbyUI::CreateSession);
    Btn_Go_Find->OnClicked.AddDynamic(this, &ULobbyUI::GoFind);
    Btn_Go_Create->OnClicked.AddDynamic(this, &ULobbyUI::GoCreate);
    Btn_FindSession->OnClicked.AddDynamic(this, &ULobbyUI::FindSession);

    // slider 값이 변경되면 호출되는 함수 등록
    Slider_PlayerCount->OnValueChanged.AddDynamic(this, &ULobbyUI::OnValueChanged);

    // 세션 검색되면 호출되는 함수 등록
    gi->OnAddSession.BindUObject(this, &ULobbyUI::OnAddSession);

    // 세션 완전 검색 완료 시 호출되는 함수 등록
    gi->OnFindComplete.BindUObject(this, &ULobbyUI::OnFindComplete);
}

void ULobbyUI::GoCreate()
{
    WidgetSwitcher->SetActiveWidgetIndex(1);
}

void ULobbyUI::GoFind()
{
    WidgetSwitcher->SetActiveWidgetIndex(2);

    // 강제로 검색
    FindSession();
}

void ULobbyUI::CreateSession()
{
    // edit_DisplayName 에 값이 없으면 함수 나가자
    if (Edit_DisplayName->GetText().IsEmpty()) return;
    
    // 세션 생성
    FString dispalyName = Edit_DisplayName->GetText().ToString();
    int32 playerCount = Slider_PlayerCount->GetValue();
    gi->CreateMySession(dispalyName, playerCount);
}

void ULobbyUI::OnValueChanged(float value)
{
    // 최대 인원  Text 수정
    text_PlayerCount->SetText(FText::AsNumber(value));
}

void ULobbyUI::FindSession()
{
    // 세션 목록 다 지우기
    Scroll_SessionList->ClearChildren();

    gi->FindOtherSession();
}

void ULobbyUI::OnAddSession(int32 idx, FString info)
{
    USessionItem* item = CreateWidget<USessionItem>(GetWorld(), SessionItemFactory);
    Scroll_SessionList->AddChild(item);
    item->SetInfo(idx, info);
}

void ULobbyUI::OnFindComplete(bool isComplete)
{
    // 검색 버튼 활성/비활성
    Btn_FindSession->SetIsEnabled(isComplete);
}
