// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MenuInterface.h"
#include "InGameMenu.generated.h"

/**
 *  BINDING SETUP IN LEVEL BLUEPRINT
 */
UCLASS()
class FPSGAME_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMenuInterface(IMenuInterface* MenuInterface);
	
	void Setup();

	void Teardown();

protected:
	virtual bool Initialize();

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Cancel;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_QuitGame;

	UFUNCTION()
	void CancelPressed();

	UFUNCTION()
	void QuitPressed();

	IMenuInterface* MenuInterface;
};
