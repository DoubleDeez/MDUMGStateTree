#pragma once

#include "CoreMinimal.h"
#include "MDUMGStateTreeTaskBase.h"
#include "MDUMGStateTreeTask_PlayAnim.generated.h"

class UUserWidget;
class UWidgetAnimation;
namespace EUMGSequencePlayMode { enum Type; }

USTRUCT()
struct FMDUMGStateTreeTask_PlayAnim_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Context")
	TObjectPtr<UUserWidget> Widget = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<UWidgetAnimation> Animation = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float StartAtTime = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	int32 NumberOfLoops = 1;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TEnumAsByte<EUMGSequencePlayMode::Type> PlayMode;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float PlaybackSpeed = 1.f;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bRestoreState = false;
	
};

/**
 * Play a User Widget Animation
 */
USTRUCT(meta = (DisplayName = "Play Widget Animation"))
struct MDUMGSTATETREE_API FMDUMGStateTreeTask_PlayAnim : public FMDUMGStateTreeTaskBase
{
	GENERATED_BODY()
	
	typedef FMDUMGStateTreeTask_PlayAnim_InstanceData FInstanceDataType;
	
public:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};
