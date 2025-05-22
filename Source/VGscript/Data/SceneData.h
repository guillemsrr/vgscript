// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "VgScript/Data/Structs/Action/ActionData.h"
#include "VgScript/Data/Structs/Condition/ConditionGroup.h"
#include "VgScript/Data/Structs/Decision/DecisionLines.h"
#include "VgScript/Data/Structs/Talk/TalkLines.h"

#include "SceneData.generated.h"

USTRUCT()
struct VGSCRIPT_API FSceneData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FName Name;

	UPROPERTY(VisibleAnywhere, meta = (ForceInlineRow))
	TMap<FString, FTalkLines> TalkDataMap;

	UPROPERTY(VisibleAnywhere, meta = (ForceInlineRow))
	TMap<FString, FDecisionLines> DecisionsDataMap;

	UPROPERTY(VisibleAnywhere, meta = (ForceInlineRow))
	TMap<FString, FActionData> ActionsDataMap;

	UPROPERTY(VisibleAnywhere, meta = (ForceInlineRow))
	TMap<FString, FConditionGroup> ConditionsDataMap;
};