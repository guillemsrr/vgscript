// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "VgScript/Data/SceneKeyCode.h"
#include "VgScript/Data/Structs/Condition/InlineConditionData.h"

#include "DecisionLine.generated.h"

USTRUCT()
struct VGSCRIPT_API FDecisionLine
{
	GENERATED_BODY()

	FDecisionLine() = default;

	explicit FDecisionLine(const FSceneKeyCode& InKey)
	{
		this->SceneKey = InKey;
	}

	UPROPERTY(VisibleAnywhere)
	FSceneKeyCode SceneKey;

	UPROPERTY(VisibleAnywhere)
	FString SourceText;

	UPROPERTY(VisibleAnywhere)
	FString Hash;

	UPROPERTY(VisibleAnywhere)
	FText Text;

	UPROPERTY(VisibleAnywhere)
	FInlineConditionData Condition;

	UPROPERTY(VisibleDefaultsOnly)
	TArray<FActionValue> Modifiers;

	UPROPERTY(VisibleAnywhere)
	TArray<FSceneKeyCode> NextKeys;

	UPROPERTY(VisibleAnywhere)
	TArray<FInlineActionData> NextInlineActionDatas;

	//here because this struct is the same that's being used
	//TODO: just separate
	bool bIsHold = false;
	float HoldTimeRelation = 1.f;
	bool bIsRemoved = false;
	bool bIsRepeated = false;
	bool bCanRepeat = true;

	void Copy(const FDecisionLine* DecisionLine);

	bool operator==(const FDecisionLine& Other) const
	{
		return SceneKey == Other.SceneKey;

		/*&& SourceText == Other.SourceText && Hash == Other.Hash && Text.ToString() ==
			Other.Text.ToString() && Modifiers == Other.Modifiers && NextKeys == Other.NextKeys && NextInlineActionDatas
			== Other.NextInlineActionDatas && bIsHold == Other.bIsHold && bIsRemoved == Other.bIsRemoved && bCanRepeat
			== Other.bCanRepeat*/
	}
};