// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "ConditionData.h"

#include "ConditionGroup.generated.h"

USTRUCT()
struct VGSCRIPT_API FConditionGroup: public FActionDataBase
{
	GENERATED_BODY()
	
	using FActionDataBase::FActionDataBase;

	UPROPERTY(VisibleAnywhere, meta = (TitleProperty = "ConditionsActionData.CodeName"))
	TArray<FConditionData> Conditions;
};