#include "MDUMGStateTreeSchema.h"

#include "MovieScene.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tasks/MDUMGStateTreeTaskBase.h"

#if WITH_EDITORONLY_DATA
#include "WidgetBlueprint.h"
#endif

UMDUMGStateTreeSchema::UMDUMGStateTreeSchema()
	: ContextWidgetClass(UUserWidget::StaticClass())
{
	// TODO - Add Animations + SubWidgets?

	WidgetContextDataDesc = {
		{ FName("OwningWidget"), UUserWidget::StaticClass(), FGuid::NewGuid() }
	};

	BuildContextDataDesc();
}

void UMDUMGStateTreeSchema::PostLoad()
{
	Super::PostLoad();

	SyncWidgetClass();
	SyncUserContextItems();
}

#if WITH_EDITOR
void UMDUMGStateTreeSchema::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (const FProperty* Property = PropertyChangedEvent.Property)
	{
		if (Property->GetOwnerClass() == StaticClass() && Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMDUMGStateTreeSchema, ContextWidgetClass))
		{
			SyncWidgetClass();
		}
		else if (Property->GetOwnerStruct() == FMDUMGStateTreeUserContextItem::StaticStruct()
			|| Property->GetOwnerClass() == StaticClass() && Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMDUMGStateTreeSchema, AdditionalContext))
		{
			SyncUserContextItems();
		}
	}
}
#endif

bool UMDUMGStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return InScriptStruct->IsChildOf(FStateTreeConditionCommonBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FStateTreeEvaluatorCommonBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FStateTreeTaskCommonBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FMDUMGStateTreeTaskBase::StaticStruct());
}

bool UMDUMGStateTreeSchema::IsClassAllowed(const UClass* InScriptStruct) const
{
	return IsChildOfBlueprintBase(InScriptStruct);
}

bool UMDUMGStateTreeSchema::IsExternalItemAllowed(const UStruct& InStruct) const
{
	return InStruct.IsChildOf(UWidget::StaticClass())
		|| InStruct.IsChildOf(AGameModeBase::StaticClass())
		|| InStruct.IsChildOf(AGameStateBase::StaticClass())
		|| InStruct.IsChildOf(AActor::StaticClass())
		|| InStruct.IsChildOf(APlayerController::StaticClass())
		|| InStruct.IsChildOf(APlayerState::StaticClass())
		|| InStruct.IsChildOf(APawn::StaticClass())
		|| InStruct.IsChildOf(ULocalPlayer::StaticClass())
		|| InStruct.IsChildOf(UWorldSubsystem::StaticClass())
		|| InStruct.IsChildOf(ULocalPlayerSubsystem::StaticClass())
		|| InStruct.IsChildOf(UGameInstanceSubsystem::StaticClass())
		|| InStruct.IsChildOf(UWidgetAnimation::StaticClass());
}

TConstArrayView<FStateTreeExternalDataDesc> UMDUMGStateTreeSchema::GetContextDataDescs() const
{
	return ContextDataDesc;
}

void UMDUMGStateTreeSchema::SyncUserContextItems()
{
	UserContextDataDesc.SetNum(AdditionalContext.Num());

	for (int32 i = 0; i < AdditionalContext.Num(); ++i)
	{
		UserContextDataDesc[i] = AdditionalContext[i];
	}

	UserContextDataDesc.RemoveAll([](const FStateTreeExternalDataDesc& Data)
	{
		return Data.Struct == nullptr;
	});

	BuildContextDataDesc();
}

void UMDUMGStateTreeSchema::SyncWidgetClass()
{
	WidgetContextDataDesc[0].Struct = ContextWidgetClass;

	if (ContextWidgetClass != nullptr)
	{
		if (const UWidgetBlueprintGeneratedClass* WidgetBPClass = Cast<UWidgetBlueprintGeneratedClass>(ContextWidgetClass))
		{
#if WITH_EDITORONLY_DATA
			if (UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(WidgetBPClass->ClassGeneratedBy))
			{
				if (!WidgetBP->OnCompiled().IsBoundToObject(this))
				{
					WidgetBP->OnCompiled().AddUObject(this, &UMDUMGStateTreeSchema::OnWidgetCompiled);
				}
			}
#endif

			// Animations
			{
				TSet<FName> AnimationNames;
				Algo::TransformIf(WidgetBPClass->Animations, AnimationNames,
					[](const UWidgetAnimation* Anim)
				{
					return Anim != nullptr && Anim->GetMovieScene() != nullptr;
				},
					[](const UWidgetAnimation* Anim)
				{
					return Anim->GetMovieScene()->GetFName();
				});

				// TODO - Detect renames of animations since this will unlink if an animation is renamed
				WidgetContextDataDesc.RemoveAll([&AnimationNames](const FStateTreeExternalDataDesc& Data)
				{
					return Data.Struct == UWidgetAnimation::StaticClass() && !AnimationNames.Contains(Data.Name);
				});

				for (const FName& AnimName : AnimationNames)
				{
					const bool bHasAnimation = WidgetContextDataDesc.ContainsByPredicate([&AnimName](const FStateTreeExternalDataDesc& Data)
					{
						return Data.Name == AnimName;
					});
					if (!bHasAnimation)
					{
						WidgetContextDataDesc.Add({ AnimName, UWidgetAnimation::StaticClass(), FGuid::NewGuid() });
					}
				}
			}

			// Widgets
			{
				TMap<FName, TSubclassOf<UWidget>> WidgetClassMap;
				if (UWidgetTree* WidgetTree = WidgetBPClass->GetWidgetTreeArchetype())
				{
					WidgetTree->ForEachWidget([&](UWidget* Widget)
					{
						if (Widget != nullptr && Widget->bIsVariable)
						{
							WidgetClassMap.Add(Widget->GetFName(), Widget->GetClass());
						}
					});
				}
				
				// TODO - Detect renames of widgets since this will unlink if an animation is renamed
				WidgetContextDataDesc.RemoveAll([&WidgetClassMap](const FStateTreeExternalDataDesc& Data)
				{
					return Data.Struct->IsChildOf(UWidget::StaticClass()) && !WidgetClassMap.Contains(Data.Name) && Data.Name != FName("OwningWidget");
				});

				for (const auto& Pair : WidgetClassMap)
				{
					FStateTreeExternalDataDesc* FoundWidgetData = WidgetContextDataDesc.FindByPredicate([WidgetName = Pair.Key](const FStateTreeExternalDataDesc& Data)
					{
						return Data.Name == WidgetName;
					});
					if (FoundWidgetData != nullptr)
					{
						FoundWidgetData->Struct = Pair.Value;
					}
					else
					{
						WidgetContextDataDesc.Add({ Pair.Key, Pair.Value, FGuid::NewGuid() });
					}
				}
			}
		}
		else
		{
			WidgetContextDataDesc.SetNum(1);
		}
	}

	BuildContextDataDesc();
}

void UMDUMGStateTreeSchema::BuildContextDataDesc()
{
	ContextDataDesc = WidgetContextDataDesc;
	ContextDataDesc.Append(UserContextDataDesc);
}

#if WITH_EDITORONLY_DATA
void UMDUMGStateTreeSchema::OnWidgetCompiled(UBlueprint* WidgetBP)
{
	SyncWidgetClass();
}
#endif
