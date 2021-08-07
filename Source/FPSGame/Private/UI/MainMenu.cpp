// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"


bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(Button_Host != nullptr)) return false;
	Button_Host->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(Button_Join != nullptr)) return false;
	Button_Join->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!ensure(Button_JoinMenu != nullptr)) return false;
	Button_JoinMenu->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(Button_CancelJoinMenu != nullptr)) return false;
	Button_CancelJoinMenu->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(Button_ExitGame != nullptr)) return false;
	Button_ExitGame->OnClicked.AddDynamic(this, &UMainMenu::ExitGamePressed);

	return true;
}

void UMainMenu::SetMenuInterface(IMenuInterface* MenuInterface)
{
	this->MenuInterface = MenuInterface;
}

void UMainMenu::Setup()
{
	// Add to the Viewport
	this->AddToViewport();

	// Find Player Controller

	// Get World
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	// Get PlayerController from World
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);

	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = true;
}


void UMainMenu::Teardown()
{
	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = false;
}

void UMainMenu::HostServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Host Server"));

	if (MenuInterface != nullptr)
	{
		MenuInterface->Host();
	}
}

void UMainMenu::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Join Server"));

	if (MenuInterface != nullptr)
	{
		
		if (!ensure(IPAddressField != nullptr)) return;
		const FString& Address = IPAddressField->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("IP Address: %s"), *Address);
		
		MenuInterface->Join(Address);
	}
}

void UMainMenu::OpenJoinMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Open Join Menu"));

	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	
	MenuSwitcher->SetActiveWidget(JoinMenu);
}

void UMainMenu::OpenMainMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Cancel Join Menu"));

	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::ExitGamePressed()
{
	// Exit Game

	// Get World
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	// Get PlayerController from World
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand("quit");
}