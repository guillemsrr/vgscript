// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "InlineActionData.h"

#include "VgScript/Data/Structs/Base/ActionDataBase.h"

#include "ActionData.generated.h"

USTRUCT()
struct VGSCRIPT_API FActionData : public FActionDataBase
{
	GENERATED_BODY()

	using FActionDataBase::FActionDataBase;
	
	UPROPERTY(VisibleAnywhere)
	FInlineActionData Data;

	UPROPERTY(VisibleAnywhere)
	TArray<FSceneKeyCode> NextKeys;
	UPROPERTY(VisibleAnywhere)
	TArray<FInlineActionData> NextInlineActionDatas;

	UPROPERTY(VisibleAnywhere)
	TArray<FSceneKeyCode> ParallelKeys;
	UPROPERTY(VisibleAnywhere)
	TArray<FInlineActionData> ParallelInlineActionDatas;
};