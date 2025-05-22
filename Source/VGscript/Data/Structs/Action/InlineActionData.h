// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "ActionValue.h"

#include "InlineActionData.generated.h"

USTRUCT()
struct VGSCRIPT_API FInlineActionData
{
	GENERATED_BODY()

	FInlineActionData() = default;

	explicit FInlineActionData(FString InCodeName);

	UPROPERTY(EditAnywhere)
	FString CodeName;
	UPROPERTY(EditAnywhere)
	TArray<FActionValue> Values;

	bool IsEmpty() const
	{
		return CodeName.IsEmpty() && Values.IsEmpty();
	}
};

inline FInlineActionData::FInlineActionData(FString InCodeName)
{
	this->CodeName = InCodeName;
}