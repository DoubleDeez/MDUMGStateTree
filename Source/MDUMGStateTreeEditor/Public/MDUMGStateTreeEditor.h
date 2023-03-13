#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMDUMGStateTreeEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedRef<FExtender> CheckAddStateTreeToolbarButton(const TSharedRef<FUICommandList> Commands, const TArray<UObject*> Objects) const;
	void AddStateTreeToolbarButton(FToolBarBuilder& ToolBarBuilder, TWeakObjectPtr<UObject> EditorObject) const;
};
