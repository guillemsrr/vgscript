// Copyright (c) Guillem Serra. All Rights Reserved.

#include "DecisionLines.h"

int32 FDecisionLines::GetLineIndex(const FSceneKeyCode& InSceneKeyCode)
{
	int32 Index = 0;
	for (const FDecisionLine& Line : Lines)
	{
		if (Line.SceneKey == InSceneKeyCode)
		{
			return Index;
		}

		Index++;
	}

	return INDEX_NONE;
}