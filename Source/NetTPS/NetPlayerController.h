#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPlayerController.generated.h"

UCLASS()
class NETTPS_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    UFUNCTION(Server, Reliable)
    void ServerRPC_ChangeToSpectator();

    void RespawnPlayer();

public:
    virtual void Tick(float DeltaSeconds) override;
};
