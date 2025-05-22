// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "InlineConditionData.h"

#include "VgScript/Data/Structs/Base/ActionDataBase.h"
#include "ConditionData.generated.h"

USTRUCT()
struct VGSCRIPT_API FConditionData: public FActionDataBase
{
	GENERATED_BODY()

	using FActionDataBase::FActionDataBase;

	UPROPERTY(VisibleAnywhere)
	FInlineConditionData InlineConditionData;

	UPROPERTY(VisibleAnywhere)
	TArray<FSceneKeyCode> ResultKeys;
};