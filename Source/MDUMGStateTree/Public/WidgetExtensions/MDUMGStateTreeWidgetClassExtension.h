#pragma once

#include "CoreMinimal.h"
#include "Extensions/WidgetBlueprintGeneratedClassExtension.h"
#include "MDUMGStateTreeWidgetClassExtension.generated.h"

struct FStateTreeReference;

/**
 * 
 */
UCLASS()
class MDUMGSTATETREE_API UMDUMGStateTreeWidgetClassExtension : public UWidgetBlueprintGeneratedClassExtension
{
	GENERATED_BODY()

public:
	virtual void Initialize(UUserWidget* UserWidget) override;
	
	void SetStateTrees(const TArray<FStateTreeReference>& InStateTrees);

protected:
	UPROPERTY()
	TArray<FStateTreeReference> StateTrees;
};
