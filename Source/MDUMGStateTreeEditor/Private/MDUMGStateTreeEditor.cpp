#include "MDUMGStateTreeEditor.h"

#include "BlueprintEditorModule.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "WidgetBlueprint.h"
#include "WidgetExtensions/MDUMGStateTreeWidgetBlueprintExtension.h"

#define LOCTEXT_NAMESPACE "FMDUMGStateTreeEditorModule"

namespace MDUMGStateTreeEditorModule_Private
{
	TSharedRef<SWidget> CreateStateTreeWidget(TWeakObjectPtr<UObject> EditorObject)
	{
		if (UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(EditorObject.Get()))
		{
			if (UMDUMGStateTreeWidgetBlueprintExtension* StateTreeExt = UWidgetBlueprintExtension::RequestExtension<UMDUMGStateTreeWidgetBlueprintExtension>(WidgetBP))
			{
				FDetailsViewArgs DetailsViewArgs;
				DetailsViewArgs.bLockable = false;
				DetailsViewArgs.bShowPropertyMatrixButton = false;
				DetailsViewArgs.bHideSelectionTip = true;
				
				const TSharedRef<IDetailsView> DetailsView = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsViewArgs);
				DetailsView->SetObject(StateTreeExt);

				return SNew(SBox)
				.MinDesiredWidth(400.f)
				.MinDesiredHeight(300.f)
				[
					DetailsView
				];
			}
		}
		
		return SNullWidget::NullWidget;
	}
}

void FMDUMGStateTreeEditorModule::StartupModule()
{
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	BlueprintEditorModule.GetMenuExtensibilityManager()->GetExtenderDelegates().Add(FAssetEditorExtender::CreateRaw(this, &FMDUMGStateTreeEditorModule::CheckAddStateTreeToolbarButton));
}

void FMDUMGStateTreeEditorModule::ShutdownModule()
{
    
}

TSharedRef<FExtender> FMDUMGStateTreeEditorModule::CheckAddStateTreeToolbarButton(const TSharedRef<FUICommandList> Commands, const TArray<UObject*> Objects) const
{
	TSharedRef<FExtender> Extender = MakeShareable(new FExtender());

	if (UObject* EditorObject = (Objects.Num() > 0) ? Objects[0] : nullptr)
	{
		if (EditorObject != nullptr && EditorObject->IsA<UWidgetBlueprint>())
		{
			Extender->AddToolBarExtension(TEXT("Asset"), EExtensionHook::After, Commands
					, FToolBarExtensionDelegate::CreateRaw(this, &FMDUMGStateTreeEditorModule::AddStateTreeToolbarButton, MakeWeakObjectPtr(EditorObject)));
		}	
	}
	
	return Extender;	
}

void FMDUMGStateTreeEditorModule::AddStateTreeToolbarButton(FToolBarBuilder& ToolBarBuilder, TWeakObjectPtr<UObject> EditorObject) const
{
	ToolBarBuilder.AddComboButton(
		FUIAction()
		, FOnGetContent::CreateStatic(MDUMGStateTreeEditorModule_Private::CreateStateTreeWidget, EditorObject)
		, LOCTEXT("StateTreeButtonLabel", "State Trees")
		, LOCTEXT("StateTreeButtonLabelToolTip", "Assign state trees to this widget")
		, FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Outliner"));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FMDUMGStateTreeEditorModule, MDUMGStateTreeEditor)