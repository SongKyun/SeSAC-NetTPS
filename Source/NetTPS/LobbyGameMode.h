#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class NETTPS_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<class ULobbyUI> lobbyUIFactory;

    UPROPERTY()
    class ULobbyUI* lobbyUI;
};
