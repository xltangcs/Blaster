// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();	// ���ø���� NativeDestruct() ����
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
				-1,				// ʹ�� -1 ���Ḳ��ǰ��ĵ�����Ϣ
				15.f,			// ������Ϣ����ʾʱ��
				FColor::Yellow,	// ������ɫ����ɫ
				FString::Printf(TEXT("Session created Successfully!"))	// ��ӡ�Ự�����ɹ���Ϣ
			);
		}

		// �Ự�����ɹ��������ؿ� Lobby
		UWorld* World = GetWorld();
		if (World) {
			// Uworld->ServerTravel��https://docs.unrealengine.com/5.0/en-US/API/Runtime/Engine/Engine/UWorld/ServerTravel/
			World->ServerTravel(PathToLobby);	// ��Ϊ������������ Lobby �ؿ�
		}
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
				-1,				// ʹ�� -1 ���Ḳ��ǰ��ĵ�����Ϣ
				15.f,			// ������Ϣ����ʾʱ��
				FColor::Yellow,	// ������ɫ����ɫ
				FString::Printf(TEXT("Failed to create session!"))	// ��ӡ�Ự�����ɹ���Ϣ
			);
		}

		HostButton->SetIsEnabled(true);
		
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr) {
		return;
	}

	// ������������������һ��ƥ��������ͬ�ĻỰ���Ժ���Խ��иĽ���
	for (auto Result : SearchResults) {
		FString SettingsValue;	// ����Ựƥ������
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);	// ��ȡ�Ựƥ������
		if (SettingsValue == MatchType) {						// ���ƥ��������ͬ
			MultiplayerSessionsSubsystem->JoinSession(Result);	// ������ϵͳ�ļ���Ự����
			return;
		}
	}

	if(!bWasSuccessful || SearchResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}

}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();						// ��ȡ��ǰ��������ϵͳָ��
	if (OnlineSubsystem) {																// �����ǰ������ϵͳ��Ч
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();	// ��ȡ�Ự�ӿ�����ָ��
		if (SessionInterface.IsValid()) {	// �����ȡ�Ự�ӿڳɹ�
			FString Address;				// ����Ự����Դ��ַ
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);		// ��ȡ�Ự����Դ��ַ

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();	// ��ȡ��ҿ�����
			if (PlayerController) {
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);	// �ͻ��˴������ؿ� ��Lobby��
			}
		}
	}

	if(Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
	
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
	//		-1,				// ʹ�� -1 ���Ḳ��ǰ��ĵ�����Ϣ
	//		15.f,			// ������Ϣ����ʾʱ��
	//		FColor::Yellow,	// ������ɫ����ɫ
	//		FString::Printf(TEXT("Host Button Clicked!"))	// ��ӡ����¼���Ϣ
	//	);
	//}

	HostButton->SetIsEnabled(false);
	
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);	// ������Ϸ�Ự
		//UWorld* World = GetWorld();
		//if (World) {
		//	// Uworld->ServerTravel��https://docs.unrealengine.com/5.0/en-US/API/Runtime/Engine/Engine/UWorld/ServerTravel/
		//	World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));	// ��Ϊ������������ Lobby �ؿ�
		//}
	}
}

void UMenu::JoinButtonClicked()
{
	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
	//		-1,				// ʹ�� -1 ���Ḳ��ǰ��ĵ�����Ϣ
	//		15.f,			// ������Ϣ����ʾʱ��
	//		FColor::Blue,	// ������ɫ����ɫ
	//		FString::Printf(TEXT("Join Button Clicked!"))	// ��ӡ����¼���Ϣ
	//	);
	//}
	JoinButton->SetIsEnabled(false);
	
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}


void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();	// ��ȡ��ҿ�����ָ��
		if (PlayerController) {
			FInputModeGameOnly InputModeData;				// �������ÿ��Կ�����Ϸ������ģʽ
			PlayerController->SetInputMode(InputModeData);	// ������ҿ�����������ģʽ
			PlayerController->SetShowMouseCursor(false);	// ���������
		}
	}
}

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);

	bIsFocusable = true;
	/*SetIsFocusable(true);*/
	
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeSettings;
			InputModeSettings.SetWidgetToFocus(TakeWidget());
			InputModeSettings.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PlayerController->SetInputMode(InputModeSettings);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();	
	}

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->SubsystemOnCreateSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->SubsystemOnFindSessionsCompleteDelegate.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->SubsystemOnJoinSessionCompleteDelegate.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->SubsystemOnDestroySessionCompleteDelegate.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->SubsystemOnStartSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnStartSession);
	}

}
