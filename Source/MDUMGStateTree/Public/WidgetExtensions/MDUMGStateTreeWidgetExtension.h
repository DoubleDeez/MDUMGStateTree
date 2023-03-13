#pragma once

#include "CoreMinimal.h"
#include "Extensions/UserWidgetExtension.h"
#include "MDUMGStateTreeWidgetExtension.generated.h"

struct FStateTreeExecutionContext;
struct FStateTreeInstanceData;
struct FStateTreeReference;

/**
 * 
 */
UCLASS()
class MDUMGSTATETREE_API UMDUMGStateTreeWidgetExtension : public UUserWidgetExtension
{
	GENERATED_BODY()

public:
	virtual void Construct() override;

	virtual void Destruct() override;

	virtual void Tick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual bool RequiresTick() const override { return true; }
	
	void SetStateTrees(const TArray<FStateTreeReference>& InStateTrees);

protected:
	bool PopulateContext(FStateTreeExecutionContext& Context);
	
	UPROPERTY()
	TArray<FStateTreeReference> StateTrees;
	
	UPROPERTY(Transient)
	TArray<FStateTreeInstanceData> InstanceData;
};
