#include "Tasks/MDUMGStateTreeTask_PlayAnim.h"

#include "StateTreeExecutionContext.h"
#include "Blueprint/UserWidget.h"


EStateTreeRunStatus FMDUMGStateTreeTask_PlayAnim::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	UUserWidget* Widget = InstanceData.Widget;
	UWidgetAnimation* Animation = InstanceData.Animation;
	if (Widget == nullptr || Animation == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}

	Widget->PlayAnimation(Animation, InstanceData.StartAtTime, InstanceData.NumberOfLoops, InstanceData.PlayMode, InstanceData.PlaybackSpeed, InstanceData.bRestoreState);
	return Widget->IsAnimationPlaying(Animation) ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus FMDUMGStateTreeTask_PlayAnim::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const UUserWidget* Widget = InstanceData.Widget;
	const UWidgetAnimation* Animation = InstanceData.Animation;
	if (Widget == nullptr || Animation == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (Widget->IsAnimationPlaying(Animation))
	{
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Succeeded;
}
