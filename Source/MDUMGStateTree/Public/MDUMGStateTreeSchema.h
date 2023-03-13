#pragma once

#include "CoreMinimal.h"
#include "StateTreeSchema.h"
#include "MDUMGStateTreeSchema.generated.h"

class UUserWidget;

USTRUCT()
struct FMDUMGStateTreeUserContextItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Context")
	FName Name;

	UPROPERTY(EditAnywhere, Category = "Context", meta = (AllowAbstract = "true", AllowedClasses = "/Script/Engine.GameModeBase, /Script/Engine.GameStateBase, /Script/Engine.PlayerController, /Script/Engine.PlayerState, /Script/Engine.Pawn, /Script/Engine.LocalPlayer, /Script/Engine.WorldSubsystem, /Script/Engine.LocalPlayerSubsystem, /Script/Engine.GameInstanceSubsystem"))
	TSubclassOf<UObject> Type;
	
	UPROPERTY()
	FGuid Id = FGuid::NewGuid();

	operator FStateTreeExternalDataDesc() const
	{
		return FStateTreeExternalDataDesc(Name, Type, Id);
	}
};

/**
 * 
 */
UCLASS()
class MDUMGSTATETREE_API UMDUMGStateTreeSchema : public UStateTreeSchema
{
	GENERATED_BODY()

public:
	UMDUMGStateTreeSchema();

	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;
	virtual bool IsClassAllowed(const UClass* InScriptStruct) const override;
	virtual bool IsExternalItemAllowed(const UStruct& InStruct) const override;
	
	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;

protected:
	void SyncUserContextItems();
	void SyncWidgetClass();
	void BuildContextDataDesc();
	
#if WITH_EDITORONLY_DATA
	void OnWidgetCompiled(UBlueprint* WidgetBP);
#endif
	
	// Widget class the StateTree is expected to run on. Allows binding to specific properties on that widget.
	UPROPERTY(EditAnywhere, Category="Defaults", meta = (AllowAbstract = "true"))
	TSubclassOf<UUserWidget> ContextWidgetClass;

	// Additional data that you need for your state tree
	UPROPERTY(EditAnywhere, Category="Defaults")
	TArray<FMDUMGStateTreeUserContextItem> AdditionalContext;
	
	UPROPERTY()
	TArray<FStateTreeExternalDataDesc> WidgetContextDataDesc;
	
	UPROPERTY()
	TArray<FStateTreeExternalDataDesc> UserContextDataDesc;
	
	UPROPERTY()
	TArray<FStateTreeExternalDataDesc> ContextDataDesc;
};
