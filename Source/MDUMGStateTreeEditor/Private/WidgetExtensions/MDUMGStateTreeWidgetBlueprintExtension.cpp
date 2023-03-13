#include "WidgetExtensions/MDUMGStateTreeWidgetBlueprintExtension.h"

#include "StateTreeReference.h"
#include "WidgetExtensions/MDUMGStateTreeWidgetClassExtension.h"

void UMDUMGStateTreeWidgetBlueprintExtension::HandleBeginCompilation(FWidgetBlueprintCompilerContext& InCreationContext)
{
	Super::HandleBeginCompilation(InCreationContext);

	CompilerContext = &InCreationContext;
}

void UMDUMGStateTreeWidgetBlueprintExtension::HandleFinishCompilingClass(UWidgetBlueprintGeneratedClass* Class)
{
	Super::HandleFinishCompilingClass(Class);

	StateTrees.RemoveAll([](const FStateTreeReference& TreeRef)
	{
		return TreeRef.GetStateTree() == nullptr;
	});
	
	if (CompilerContext != nullptr && StateTrees.Num() > 0)
	{
		UMDUMGStateTreeWidgetClassExtension* StateTreeClass = NewObject<UMDUMGStateTreeWidgetClassExtension>(Class);
		StateTreeClass->SetStateTrees(StateTrees);
		
		CompilerContext->AddExtension(Class, StateTreeClass);
	}
}

void UMDUMGStateTreeWidgetBlueprintExtension::HandleEndCompilation()
{
	Super::HandleEndCompilation();

	CompilerContext = nullptr;
}
