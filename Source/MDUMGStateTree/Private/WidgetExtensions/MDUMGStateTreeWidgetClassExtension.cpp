#include "WidgetExtensions/MDUMGStateTreeWidgetClassExtension.h"

#include "Blueprint/UserWidget.h"
#include "WidgetExtensions/MDUMGStateTreeWidgetExtension.h"

void UMDUMGStateTreeWidgetClassExtension::Initialize(UUserWidget* UserWidget)
{
	Super::Initialize(UserWidget);

	if (UserWidget != nullptr && StateTrees.Num() > 0)
	{
		UserWidget->AddExtension<UMDUMGStateTreeWidgetExtension>()->SetStateTrees(StateTrees);
	}
}

void UMDUMGStateTreeWidgetClassExtension::SetStateTrees(const TArray<FStateTreeReference>& InStateTrees)
{
	StateTrees = InStateTrees;
}
