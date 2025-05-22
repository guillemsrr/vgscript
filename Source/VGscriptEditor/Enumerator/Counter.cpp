// Copyright (c) Guillem Serra. All Rights Reserved.

#include "Counter.h"

TArray<FString> UCounter::Count(TObjectPtr<UActionDataAsset> DataAsset)
{
	TextCounter = 0;
	TextLineCounter = 0;
	DecisionCounter = 0;
	DecisionLineCounter = 0;

	for (const TTuple<FString, FSceneData>& SceneData : DataAsset->SceneData)
	{
		for (const TTuple<FString, FDecisionLines>& DecisionsDataMap : SceneData.Value.DecisionsDataMap)
		{
			DecisionCounter++;
			DecisionLineCounter += DecisionsDataMap.Value.Lines.Num();
		}

		for (const TTuple<FString, FTalkLines>& TalkActionsMap : SceneData.Value.TalkDataMap)
		{
			TextCounter++;
			TextLineCounter += TalkActionsMap.Value.Lines.Num();
		}
	}

	TArray<FString> Script;
	Script.Add("Texts: " + FString::FromInt(TextCounter));
	Script.Add("Text Lines Counter: " + FString::FromInt(TextLineCounter));
	Script.Add("Decisions Counter: " + FString::FromInt(DecisionCounter));
	Script.Add("Decision Lines Counter: " + FString::FromInt(DecisionLineCounter));

	return Script;
}