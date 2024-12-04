#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include <Online/OnlineSessionNames.h>
#include <Kismet/GameplayStatics.h>

void UNetGameInstance::Init()
{
    Super::Init();

    IOnlineSubsystem* subsys = IOnlineSubsystem::Get();
    if (subsys)
    {
        sessionInterface = subsys->GetSessionInterface();
        // 세션 생성 성공시 호출되는 함수 등록
        sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnCreateSessionComplete);
        // 세션 파괴 성공 시 호출되는 함수 등록
        sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnDestroySessionComplete);
        // 세션 검색 성공 시 호출되는 함수 등록
        sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::OnFindSessionsComplete);
        // 세션 참여 요청 성공시 호출되는 함수 등록
        sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnJoinSessionComplete);
    }
}

void UNetGameInstance::CreateMySession(FString displayName, int32 playerCount)
{
    // 세션을 만들기 위한 옵션 설정
    FOnlineSessionSettings sessionSettings;

    // Lan을 이용할지 여부
    FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName(); // 스팀이면 스팀 반환 아니면 널 반환
    sessionSettings.bIsLANMatch = subsysName.IsEqual(FName(TEXT("NULL")));

    // Lobby 를 사용할지 여부
    sessionSettings.bUseLobbiesIfAvailable = true;

    // 세션 검색을 허용할지 여부
    sessionSettings.bShouldAdvertise = true;

    // 세션이 진행 중일 때 참여 가능 여부
    sessionSettings.bAllowJoinInProgress = true;

    // 세션의 최대 인원 설정
    sessionSettings.NumPublicConnections = playerCount;

    // 커스텀 정보
    sessionSettings.Set(TEXT("DP_NAME"), displayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); // 키, 값, 우선순위

    // 세션 생성
    FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
    sessionInterface->CreateSession(*netID, FName(displayName), sessionSettings);// 스팀은 0으로 세팅되어 있다, 세션 네임 FName 은 같지 않아도 된다
}

void UNetGameInstance::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 생성 성공 : "), *sessionName.ToString());
        currSessionName = sessionName;

        // 세션 만든 사람(서버) 이 만들어진 세션으로 이동
        GetWorld()->ServerTravel(TEXT("/Game/Net/Level/ThirdPersonMap?listen"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 생성 실패 : "), *sessionName.ToString());
    }
}

void UNetGameInstance::DestroyMySession()
{
    UE_LOG(LogTemp, Warning, TEXT("123123123"));
    sessionInterface->DestroySession(FName(currSessionName));
}

void UNetGameInstance::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 파괴 성공 : "), *sessionName.ToString());
        // Lobby 레벨로 이동
        UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyMap"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 파괴 실패 : "), *sessionName.ToString());
    }
}

void UNetGameInstance::FindOtherSession()
{
    UE_LOG(LogTemp, Warning, TEXT("세션 검색 시작"));

    // 세션 검색 설정
    sessionSearch = MakeShared<FOnlineSessionSearch>();

    // 랜을 사용할지 여부
    FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
    UE_LOG(LogTemp, Warning, TEXT("서브시스템 이름 : %s"), *subsysName.ToString());
    sessionSearch->bIsLanQuery = subsysName.IsEqual(FName(TEXT("NULL")));

    // 활성화 되어있는 세션만 검색하자
    // SEARCH_PRESENCE 매크로로 검색
    sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    //sessionSearch->QuerySettings.Set(TEXT("DP_NAME"), FString(TEXT("SeSAC")), EOnlineComparisonOp::Equals);

    // 세션 몇개까지 검색할지
    sessionSearch->MaxSearchResults = 100;

    // 세션 검색하자
    sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UNetGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        auto results = sessionSearch->SearchResults;

        for (int i = 0; i < results.Num(); i++)
        {
            FOnlineSessionSearchResult sr = results[i];

            // DP_NAME 가져오자
            FString displayName;
            sr.Session.SessionSettings.Get(TEXT("DP_NAME"), displayName);

            // 세션을 만들 사람 이름
            FString SessionCreator = sr.Session.OwningUserName;
            FString SessionInfo = FString::Printf(TEXT("%s : %s"), *displayName, *SessionCreator);
            UE_LOG(LogTemp, Warning, TEXT("세션 : %s, 만든이 : %s"), *displayName, *SessionCreator);

            // 세션 정보를 넘겨서 SessionItem을 추가하게 하자.
            OnAddSession.ExecuteIfBound(i, SessionInfo);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("세션 검색 완료"));
}

void UNetGameInstance::JoinOtherSession(int32 idx)
{
    auto results = sessionSearch->SearchResults;

    // 세션 이름
    FString displayName;
    results[idx].Session.SessionSettings.Get(TEXT("DP_NAME"), displayName);

    // 세션 참여
    sessionInterface->JoinSession(0, FName(displayName), results[idx]);
}

void UNetGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
    if (result == EOnJoinSessionCompleteResult::Success)
    {
        currSessionName = sessionName;

        FString url;
        sessionInterface->GetResolvedConnectString(sessionName, url);

        APlayerController* pc = GetWorld()->GetFirstPlayerController();
        pc->ClientTravel(url, ETravelType::TRAVEL_Absolute);
    }
}
