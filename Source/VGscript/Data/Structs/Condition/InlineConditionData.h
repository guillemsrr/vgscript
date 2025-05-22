// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "VgScript/Data/Structs/Action/InlineActionData.h"

#include "InlineConditionData.generated.h"

USTRUCT()
struct VGSCRIPT_API FInlineConditionData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FInlineActionData> TrueConditionsActionData;
	UPROPERTY(VisibleAnywhere)
	TArray<FInlineActionData> FalseConditionsActionData;

	bool IsEmpty() const
	{
		return TrueConditionsActionData.IsEmpty() && FalseConditionsActionData.IsEmpty();
	}

	void Append(FInlineConditionData OtherCondition);
};

inline void FInlineConditionData::Append(FInlineConditionData OtherCondition)
{
	TrueConditionsActionData.Append(OtherCondition.TrueConditionsActionData);
	FalseConditionsActionData.Append(OtherCondition.FalseConditionsActionData);
}