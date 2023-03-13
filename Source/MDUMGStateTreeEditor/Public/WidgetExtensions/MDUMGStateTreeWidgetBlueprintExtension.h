#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprintExtension.h"
#include "MDUMGStateTreeWidgetBlueprintExtension.generated.h"

struct FStateTreeReference;

/**
 * 
 */
UCLASS()
class MDUMGSTATETREEEDITOR_API UMDUMGStateTreeWidgetBlueprintExtension : public UWidgetBlueprintExtension
{
	GENERATED_BODY()

protected:
	virtual void HandleBeginCompilation(FWidgetBlueprintCompilerContext& InCreationContext) override;
	virtual void HandleFinishCompilingClass(UWidgetBlueprintGeneratedClass* Class) override;
	virtual void HandleEndCompilation() override;
	
	UPROPERTY(EditAnywhere, Category = "State Tree", meta=(Schema="/Script/MDUMGStateTree.MDUMGStateTreeSchema"))
	TArray<FStateTreeReference> StateTrees;

private:
	FWidgetBlueprintCompilerContext* CompilerContext = nullptr;
};
