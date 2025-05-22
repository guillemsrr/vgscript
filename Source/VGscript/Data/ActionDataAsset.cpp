// Copyright (c) Guillem Serra. All Rights Reserved.

#include "ActionDataAsset.h"

FTalkLines* UActionDataAsset::GetTalkLines(FSceneKeyCode Key)
{
	if (!SceneData.Contains(Key.Scene))
	{
		return nullptr;
	}

	TMap<FString, FTalkLines>& TalkDataMap = SceneData[Key.Scene].TalkDataMap;
	if (!TalkDataMap.Contains(Key.Code))
	{
		return nullptr;
	}

	FTalkLines* TalkLines = &TalkDataMap[Key.Code];
	return TalkLines;
}

FTalkLine* UActionDataAsset::GetTalkLine(const FSceneKeyCode& Key)
{
	FString KeyCode = Key.ToString();
	int32 Index = KeyCode.Find("-", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString TalkKeyWithoutLine = KeyCode.Left(Index);
	FTalkLines* TalkLines = GetTalkLines(TalkKeyWithoutLine);
	if (!TalkLines)
	{
		return nullptr;
	}

	Index = TalkLines->GetLineIndex(Key);
	if (Index == INDEX_NONE)
	{
		return nullptr;
	}

	FTalkLine* TalkLine = &TalkLines->Lines[Index];
	return TalkLine;
}

FDecisionLines* UActionDataAsset::GetDecisionLines(FSceneKeyCode Key)
{
	if (!SceneData.Contains(Key.Scene))
	{
		return nullptr;
	}

	TMap<FString, FDecisionLines>& DecisionDataMap = SceneData[Key.Scene].DecisionsDataMap;
	if (!DecisionDataMap.Contains(Key.Code))
	{
		return nullptr;
	}

	FDecisionLines* DecisionLines = &DecisionDataMap[Key.Code];
	return DecisionLines;
}

FDecisionLine* UActionDataAsset::GetDecisionLine(FSceneKeyCode Key)
{
	FString KeyCode = Key.ToString();
	int32 Index = KeyCode.Find("-", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString KeyWithoutLine = KeyCode.Left(Index);
	FDecisionLines* DecisionLines = GetDecisionLines(KeyWithoutLine);
	if (!DecisionLines)
	{
		return nullptr;
	}

	Index = DecisionLines->GetLineIndex(Key);
	if (Index == INDEX_NONE)
	{
		return nullptr;
	}

	FDecisionLine* DecisionLine = &DecisionLines->Lines[Index];
	return DecisionLine;
}