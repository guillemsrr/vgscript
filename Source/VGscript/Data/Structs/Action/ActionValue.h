// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "VgScript/Data/SceneKeyCode.h"

#include "ActionValue.generated.h"

USTRUCT(BlueprintType)
struct VGSCRIPT_API FActionValue
{
	GENERATED_BODY()

	FActionValue() = default;

	FActionValue(FString InValue)
	{
		this->Value = InValue;
	}

	FActionValue(FString InCodeName, FString InValue)
	{
		this->CodeName = InCodeName;
		this->Value = InValue;
	}

	bool IsJustASceneKeyCode() const
	{
		if (!CodeName.Equals("SceneKeyCode") && !CodeName.IsEmpty())
		{
			return false;
		}

		return FSceneKeyCode::IsValid(Value);
	}

	FString ToString() const;
	bool IsEmpty() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CodeName;

	//TODO: could be an array?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Value;
};