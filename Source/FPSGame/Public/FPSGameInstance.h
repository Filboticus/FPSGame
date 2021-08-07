// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UI/MenuInterface.h"
#include "FPSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API UFPSGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:

	UFPSGameInstance(const FObjectInitializer & ObjectInitializer);

	virtual void Init();

	virtual void LoadMainMenu() override;

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	UFUNCTION(BlueprintCallable)
	void LoadInGameMenu();

	UFUNCTION(Exec)
	void Host() override;

	UFUNCTION(Exec)
	void Join(const FString& Address) override;

private:
	TSubclassOf<class UUserWidget> MenuClass;

	TSubclassOf<class UUserWidget> InGameMenuClass;

	class UMainMenu* Menu;

	class UInGameMenu* InGameMenu;
};
