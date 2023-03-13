#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MDUMGStateTreeTaskBase.generated.h"

/**
 * Base struct for all tasks from this plugin
 */
USTRUCT(meta = (Hidden))
struct MDUMGSTATETREE_API FMDUMGStateTreeTaskBase : public FStateTreeTaskBase
{
	GENERATED_BODY()
};
