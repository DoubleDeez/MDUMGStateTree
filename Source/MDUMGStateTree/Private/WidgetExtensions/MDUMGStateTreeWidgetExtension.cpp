#include "WidgetExtensions/MDUMGStateTreeWidgetExtension.h"

#include "MovieScene.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeReference.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Subsystems/WorldSubsystem.h"

void UMDUMGStateTreeWidgetExtension::Construct()
{
	Super::Construct();

	check(StateTrees.Num() == InstanceData.Num());

	for (int32 i = 0; i < StateTrees.Num(); ++i)
	{
		FStateTreeReference& StateTreeRef = StateTrees[i];
		FStateTreeExecutionContext Context(*GetUserWidget(), *StateTreeRef.GetStateTree(), InstanceData[i]);
		if (PopulateContext(Context))
		{
			Context.SetParameters(StateTreeRef.GetParameters());
			Context.Start();
		}
	}
}

void UMDUMGStateTreeWidgetExtension::Destruct()
{
	Super::Destruct();

	for (int32 i = 0; i < StateTrees.Num(); ++i)
	{
		FStateTreeReference& StateTreeRef = StateTrees[i];
		FStateTreeExecutionContext Context(*GetUserWidget(), *StateTreeRef.GetStateTree(), InstanceData[i]);
		if (PopulateContext(Context))
		{
			Context.Stop();
		}
	}
}

void UMDUMGStateTreeWidgetExtension::Tick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::Tick(MyGeometry, InDeltaTime);

	for (int32 i = 0; i < StateTrees.Num(); ++i)
	{
		FStateTreeReference& StateTreeRef = StateTrees[i];
		FStateTreeExecutionContext Context(*GetUserWidget(), *StateTreeRef.GetStateTree(), InstanceData[i]);
		if (PopulateContext(Context))
		{
			Context.Tick(InDeltaTime);
		}
	}
}

void UMDUMGStateTreeWidgetExtension::SetStateTrees(const TArray<FStateTreeReference>& InStateTrees)
{
	StateTrees = InStateTrees;
	InstanceData.SetNum(StateTrees.Num());
}

bool UMDUMGStateTreeWidgetExtension::PopulateContext(FStateTreeExecutionContext& Context)
{
	UUserWidget* OwningWidget = GetUserWidget();
	if (!Context.IsValid() || OwningWidget == nullptr)
	{
		return false;
	}

	const UWorld* World = GetWorld();
	ULocalPlayer* OwningLocalPlayer = OwningWidget->GetOwningLocalPlayer();

	static const FName OwningWidgetName(TEXT("OwningWidget"));
	auto PopulateContextList = [&](const TConstArrayView<FStateTreeExternalDataDesc>& ContextList)
	{
		for (const FStateTreeExternalDataDesc& ItemDesc : ContextList)
		{
			if (ItemDesc.Struct != nullptr)
			{
				if (ItemDesc.Struct->IsChildOf(UWorldSubsystem::StaticClass()))
				{
					UWorldSubsystem* Subsystem = World != nullptr ? World->GetSubsystemBase(Cast<UClass>(const_cast<UStruct*>(ItemDesc.Struct.Get()))) : nullptr;
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(Subsystem));
				}
				else if (ItemDesc.Struct->IsChildOf(ULocalPlayerSubsystem::StaticClass()))
				{
					ULocalPlayerSubsystem* Subsystem = OwningLocalPlayer != nullptr ? OwningLocalPlayer->GetSubsystemBase(Cast<UClass>(const_cast<UStruct*>(ItemDesc.Struct.Get()))) : nullptr;
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(Subsystem));
				}
				else if (ItemDesc.Struct->IsChildOf(UGameInstanceSubsystem::StaticClass()))
				{
					const UGameInstance* GameInstance = World != nullptr ? World->GetGameInstance() : nullptr;
					UGameInstanceSubsystem* Subsystem = GameInstance != nullptr ? GameInstance->GetSubsystemBase(Cast<UClass>(const_cast<UStruct*>(ItemDesc.Struct.Get()))) : nullptr;
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(Subsystem));
				}
				else if (ItemDesc.Struct->IsChildOf(APlayerController::StaticClass()))
				{
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(OwningWidget->GetOwningPlayer()));
				}
				else if (ItemDesc.Struct->IsChildOf(APawn::StaticClass()))
				{
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(OwningWidget->GetOwningPlayerPawn()));
				}
				else if (ItemDesc.Struct->IsChildOf(ULocalPlayer::StaticClass()))
				{
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(OwningLocalPlayer));
				}
				else if (ItemDesc.Struct->IsChildOf(AGameModeBase::StaticClass()))
				{
					AGameModeBase* GameMode = World != nullptr ? World->GetAuthGameMode() : nullptr;
					if (GameMode == nullptr && World != nullptr)
					{
						if (const AGameStateBase* GameState = World->GetGameState())
						{
							GameMode = const_cast<AGameModeBase*>(GameState->GetDefaultGameMode());
						}
					}
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(GameMode));
				}
				else if (ItemDesc.Struct->IsChildOf(AGameStateBase::StaticClass()))
				{
					AGameStateBase* GameState = World != nullptr ? World->GetGameState() : nullptr;
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(GameState));
				}
				else if (ItemDesc.Struct->IsChildOf(APlayerState::StaticClass()))
				{
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(OwningWidget->GetOwningPlayerState()));
				}
				else if (ItemDesc.Struct->IsChildOf(UUserWidget::StaticClass()) && ItemDesc.Name == OwningWidgetName)
				{
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(OwningWidget));
				}
				else if (ItemDesc.Struct->IsChildOf(UWidgetAnimation::StaticClass()))
				{
					if (UWidgetBlueprintGeneratedClass* BPClass = Cast<UWidgetBlueprintGeneratedClass>(OwningWidget->GetClass()))
					{
						for (UWidgetAnimation* Anim : BPClass->Animations)
						{
							if (Anim->GetMovieScene() != nullptr && Anim->GetMovieScene()->GetFName() == ItemDesc.Name)
							{
								Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(Anim));
								break;
							}
						}
					}
				}
				else if (ItemDesc.Struct->IsChildOf(UWidget::StaticClass()))
				{
					if (OwningWidget->WidgetTree != nullptr)
					{
						UWidget* Result = nullptr;
						OwningWidget->WidgetTree->ForEachWidget([&](UWidget* Widget)
						{
							if (Result == nullptr && Widget != nullptr && Widget->GetFName() == ItemDesc.Name)
							{
								Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(Widget));
							}
						});
					}
				}
				else if (ItemDesc.Struct->IsChildOf(AActor::StaticClass()))
				{
					// Make generic actor fallback to owning player
					Context.SetExternalData(ItemDesc.Handle, FStateTreeDataView(OwningWidget->GetOwningPlayer()));
				}
			}
		}
	};

	PopulateContextList(Context.GetContextDataDescs());
	PopulateContextList(Context.GetExternalDataDescs());
	
	const bool bResult = Context.AreExternalDataViewsValid();

	UE_CLOG(!bResult, LogTemp, Error, TEXT("Missing external data requirements. StateTree [%s] on Widget [%s] will not update."), *GetNameSafe(Context.GetStateTree()), *GetNameSafe(OwningWidget));
	
	return bResult;
}
