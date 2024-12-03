#include "GameUI.h"
#include "GameFramework/PlayerState.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/EditableText.h"
#include "Components/Border.h"
#include "NetPlayerState.h"
#include "PlayerStateUI.h"
#include "ChatItem.h"
#include "NetGameState.h"

class FPlayerStateSort
{
public:
    bool operator() (const APlayerState& a, const APlayerState& b) const
    {
        return (a.GetPlayerId() < b.GetPlayerId()); // 오름차순
    }
};

class FPlayerStateUISort
{
public:
    bool operator() (const UPlayerStateUI& a, const UPlayerStateUI& b) const
    {
        return (a.playerState->GetPlayerId() < b.playerState->GetPlayerId()); // 오름차순
    }
};

void UGameUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    //// GameState 가져오자.
    //AGameStateBase* gameState = GetWorld()->GetGameState();

    //// 가져온 GameState의 모든 Player 이름을 Text_PlayerState 에 출력하자.
    //FString allPlayerState;
    //FString playerName;
    //int32 playerId;
    //int32 score;
    //APlayerState* ps;

    //// playerId를 기준으로 정렬
    //gameState->PlayerArray.Sort(FPlayerStateSort());

    //for (int32 i = 0; i < gameState->PlayerArray.Num(); i++)
    //{
    //    ps = gameState->PlayerArray[i];
    //    playerName = ps->GetPlayerName();
    //    playerId = ps->GetPlayerId();
    //    score = ps->GetScore();
    //    
    //    allPlayerState.Append(FString::Printf(TEXT("[%d] %s : %d\n"), playerId, *playerName, score));
    //}

    //Text_PlayerState->SetText(FText::FromString(allPlayerState));
}

void UGameUI::NativeConstruct()
{
    Super::NativeConstruct();
    // edit_chat 에서 텍스트를 작성하고 엔터를 쳤을 때 호출되는 함수 등록
    Edit_Chat->OnTextCommitted.AddDynamic(this, &UGameUI::OnTextBoxCommitted);

    // Empty_Border 클릭 함수 등록
    Empty_Border->OnMouseButtonDownEvent.BindUFunction(this, TEXT("OnPointerEvent"));

    // 나의 PlayerState 찾아오는 Timer 돌리자
    GetWorld()->GetTimerManager().SetTimer(playerStateHandle, this, &UGameUI::SetMyPlayerState, 0.1f, true);
}

void UGameUI::SetMyPlayerState()
{
    APlayerController* pc = GetWorld()->GetFirstPlayerController();
    if (pc)
    {
        myPlayerState = pc->GetPlayerState<ANetPlayerState>();
        if (myPlayerState)
        {
            GetWorld()->GetTimerManager().ClearTimer(playerStateHandle);
        }
    }
}

void UGameUI::AddPlayerStateUI(APlayerState* ps)
{
    if (ps == nullptr) return;

    UPlayerStateUI* psUI = CreateWidget<UPlayerStateUI>(GetWorld(), playerStateUIFactory);
    psUI->Init(Cast<ANetPlayerState>(ps));

    // 만약에 ps가 이미 추가 되어있다면 함수를 나가자
    bool isExist = allPlayerState.ContainsByPredicate([ps](UPlayerStateUI* p) {
        return p->playerState == ps;
        });
    if (isExist) return;

    // 만들어진 psUI를 나만의 Array에 추가하자
    allPlayerState.Add(psUI);
    // allPlayerState 를 PlayerId 기준으로 정렬
    allPlayerState.Sort(FPlayerStateUISort());

    // vBox_PlayerState 자식들을 Clear 하자
    vBox_PlayerState->ClearChildren();
    // allPlayerState 순서대로 추가하자
    for (int32 i = 0; i < allPlayerState.Num(); i++)
    {
        vBox_PlayerState->AddChild(allPlayerState[i]);
        // Panel에 붙히고 난 후 Slot이 생성된다
        // Vertical Box Slot을 가져오자
        UVerticalBoxSlot* slot = Cast<UVerticalBoxSlot>(allPlayerState[i]->Slot);
        // Horizontal Alignment 을 오른쪽을 하자
        slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
        slot->SetPadding(FMargin(0, 0, 20, 0));
    }
}

void UGameUI::OnTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    // 만약에 Enter를 쳤다면
    if (CommitMethod == ETextCommit::OnEnter)
    {
        float scrollOffset = Scroll_Chat->GetScrollOffset();

        float scrollEndofOffset = Scroll_Chat->GetScrollOffsetOfEnd();

        // chatItem 을 하나 만든다
        UChatItem* chatItem = CreateWidget<UChatItem>(GetWorld(), chatItemFactory);
        // 채팅 내용을 --- > 닉네임 : 안녕하세요
        FString chat = FString::Printf(TEXT("%s : %s"), *myPlayerState->GetPlayerName(), *Text.ToString());


        // 만들어진 chatItem에 내용을 셋팅
        chatItem->SetContent(FText::FromString(chat));

        // scrollBox 자식으로 설정
        Scroll_Chat->AddChild(chatItem);

        // 만약에 스크롤이 맨 끝이라면
        if (scrollOffset == scrollEndofOffset)
        {
            // 스크롤을 강제로 끝으로 이동 시키자
            //Scroll_Chat->ScrollToEnd();
            // 개행이된 내용은 맨끝으로 이동을 하지 않는 문제 때문에 0.1초 뒤에 다시 한 번 강제로 끝으로 이동!
            FTimerHandle handle;
            GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
                // 다시 한 번 강제로 끝으로 이동
                Scroll_Chat->ScrollToEnd();
                }, 0.01f, false);
        }

        // edit_chat 내용을 초기화
        Edit_Chat->SetText(FText());
    }
    // 만약에 Enter를 친 후 포커스를 밀었다면
    else if (CommitMethod == ETextCommit::OnCleared)
    {
        Edit_Chat->SetFocus();
    }
    else if (CommitMethod == ETextCommit::OnUserMovedFocus)
    {
        UE_LOG(LogTemp, Warning, TEXT("포커스 변경 됨"));
    }
}

FEventReply UGameUI::OnPointerEvent(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
    Cast<ANetGameState>(GetWorld()->GetGameState())->ShowCursor(false);

    return FEventReply();
}
