// Copyright (c) Guillem Serra. All Rights Reserved.

#include "DecisionLine.h"

void FDecisionLine::Copy(const FDecisionLine* DecisionLine)
{
	SourceText = DecisionLine->SourceText;
	Hash = DecisionLine->Hash;
	Text = DecisionLine->Text;

	if (Condition.IsEmpty())
	{
		Condition = DecisionLine->Condition;
	}

	if (Modifiers.IsEmpty())
	{
		Modifiers = DecisionLine->Modifiers;
	}

	if (NextKeys.IsEmpty())
	{
		NextKeys = DecisionLine->NextKeys;
	}

	if (NextInlineActionDatas.IsEmpty())
	{
		NextInlineActionDatas = DecisionLine->NextInlineActionDatas;
	}
}