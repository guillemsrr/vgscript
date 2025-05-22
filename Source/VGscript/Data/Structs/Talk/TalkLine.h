// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "VgScript/Data/SceneKeyCode.h"

#include "VgScript/Data/Structs/Condition/InlineConditionData.h"

#include "TalkLine.generated.h"

USTRUCT()
struct VGSCRIPT_API FTalkLine
{
	GENERATED_BODY()

	FTalkLine() = default;

	explicit FTalkLine(const FSceneKeyCode& InKey)
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
	FString DifferentAudioHash;

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

	UPROPERTY(VisibleAnywhere)
	TArray<FSceneKeyCode> ParallelKeys;

	UPROPERTY(VisibleAnywhere)
	TArray<FInlineActionData> ParallelInlineActionDatas;

	UPROPERTY(VisibleAnywhere)
	FString Comment;

	bool operator==(const FTalkLine& Other) const
	{
		return Hash == Other.Hash;
	}

	void Copy(const FTalkLine* TalkLine);
	FString GetAudioHash();
};