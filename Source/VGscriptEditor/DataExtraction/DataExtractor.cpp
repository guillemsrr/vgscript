// Copyright (c) Guillem Serra. All Rights Reserved.

#include "DataExtractor.h"

#include "VgScript/Data/ActionDataAsset.h"
#include "VgScript/Data/Structs/Decision/DecisionLine.h"
#include "VgScript/Data/Structs/Talk/TalkLine.h"

#include "VgScriptEditor/Core/CodeSymbols.h"
#include "VgScriptEditor/Core/ScriptLineHandler.h"

#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"

#include "Kismet/KismetStringTableLibrary.h"

void UDataExtractor::SetActionsData(UActionDataAsset* InActionsDataAsset)
{
	this->ActionsDataAsset = InActionsDataAsset;
}

void UDataExtractor::ExtractLineData(FString Line)
{
	if (Line.Contains(FCodeSymbols::CommentLine) && !Line.Contains(FCodeSymbols::ActorCommentCode))
	{
		int32 CommentIndex = Line.Find(FCodeSymbols::CommentLine);
		if (CommentIndex != INDEX_NONE)
		{
			Line.LeftInline(CommentIndex);
			if (Line.IsEmpty())
			{
				return;
			}
		}
	}

	FString Key = FScriptLineHandler::ExtractKey(Line);
	if (Key.IsEmpty())
	{
		if (Line.StartsWith(FCodeSymbols::ActionValuesStart))
		{
			AddActionValue(Line);
		}
		return;
	}
	if (Key.StartsWith(FCodeSymbols::SceneCode))
	{
		CreateScene(Key, Line);
	}
	else if (Key.StartsWith(FCodeSymbols::PersonTalkingCode))
	{
		CreateCharacter(Line);
	}
	else if (Key.StartsWith(FCodeSymbols::TalkCode))
	{
		CreateText(Key, Line);
	}
	else if (Key.StartsWith(FCodeSymbols::DecisionCode))
	{
		CreateDecision(Key, Line);
	}
	else if (Key.StartsWith(FCodeSymbols::ActionCode))
	{
		CreateAction(Key, Line);
	}
	else if (Key.StartsWith(FCodeSymbols::ConditionCode))
	{
		CreateConditional(Key, Line);
	}
	else
	{
		GLog->Log("What's there? -> " + Line);
	}
}

void UDataExtractor::UpdateUnfinishedActions()
{
	for (TTuple<FSceneKeyCode, FSceneKeyCode> ToUpdate : CodesToUpdate)
	{
		if (ToUpdate.Key.Contains("T"))
		{
			FTalkLine* TalkLine = ActionsDataAsset->GetTalkLine(ToUpdate.Key);
			if (!TalkLine)
			{
				continue;
			}
			FTalkLine* TalkLineToCopy = ActionsDataAsset->GetTalkLine(ToUpdate.Value);
			if (!TalkLineToCopy)
			{
				continue;
			}
			TalkLine->Copy(TalkLineToCopy);
		}
		else if (ToUpdate.Key.Contains("D"))
		{
			FDecisionLine* DecisionLine = ActionsDataAsset->GetDecisionLine(ToUpdate.Key);
			if (!DecisionLine)
			{
				continue;
			}

			FDecisionLine* DecisionLineToCopy = ActionsDataAsset->GetDecisionLine(ToUpdate.Value);
			if (!DecisionLineToCopy)
			{
				continue;
			}

			DecisionLine->Copy(DecisionLineToCopy);
		}
	}
}

void UDataExtractor::CreateScene(const FString& Key, FString& Line)
{
	SceneKey = FKeyCode(Key);

	int32 TitleIndex = Line.Find(FCodeSymbols::CodeEnd);
	FString SceneName = Line.RightChop(TitleIndex + 1);
	SceneName.TrimStartAndEndInline();

	if (!ActionsDataAsset->SceneData.Contains(SceneKey))
	{
		ActionsDataAsset->SceneData.Add(SceneKey.ToString());
	}

	ActionsDataAsset->SceneData[SceneKey].Name = FName(SceneName);
}

void UDataExtractor::CreateCharacter(const FString& Line)
{
	FString Name = Line;
	Name.TrimStartAndEndInline();
	SpeakerName = FName(Name);
	int32 IndexSeparation = Line.Find(FCodeSymbols::CodeSeparation);
	if (IndexSeparation == INDEX_NONE)
	{
		return;
	}

	Name = Line.LeftChop(IndexSeparation);
	Name.TrimStartAndEndInline();
	SpeakerName = FName(Name);

	Name = Line.RightChop(IndexSeparation);
	Name.TrimStartAndEndInline();
	DirectedAtName = FName(Name);
}

void UDataExtractor::CreateText(const FString& Key, FString& Line)
{
	const FSceneKeyCode ParentKey = FScriptLineHandler::GetParentSceneKey(Key, SceneKey);
	if (!ActionsDataAsset->SceneData[ParentKey.Scene].TalkDataMap.Contains(ParentKey.Code))
	{
		FTalkLines NewTalkLines = FTalkLines(ParentKey);
		NewTalkLines.Talker = SpeakerName;
		NewTalkLines.DirectedTo = DirectedAtName;
		ActionsDataAsset->SceneData[ParentKey.Scene].TalkDataMap.Add(ParentKey.Code, NewTalkLines);
	}

	FTalkLines* TalkLines = &ActionsDataAsset->SceneData[ParentKey.Scene].TalkDataMap[ParentKey.Code];
	LastActionParentKey = ParentKey;

	FTalkLine TalkLine = FTalkLine(FScriptLineHandler::ApplySceneKeyIfNoSceneAlready(SceneKey, Key));
	TalkLine.Condition = GetInlineConditionData(Line);
	GetNextActions(Line, TalkLine.NextKeys, TalkLine.NextInlineActionDatas);
	GetParallelActions(Line, TalkLine.ParallelKeys, TalkLine.ParallelInlineActionDatas);

	FString Comment = GetActorComment(Line);
	TalkLine.Modifiers = GetModifiers(Line);

	if (Line.Contains(FCodeSymbols::SameAsCode))
	{
		FSceneKeyCode ToCopy = ExtractSameAsCode(Line);
		CodesToUpdate.Add(TalkLine.SceneKey, ToCopy);

		FString CopiedText = FScriptLineHandler::ExtractElementBetween(Line,
		                                                               FCodeSymbols::TextCode,
		                                                               FCodeSymbols::TextCode,
		                                                               ESearchDir::FromEnd);
		TalkLine.SourceText = CopiedText;
	}
	else
	{
		TalkLine.SourceText = ExtractText(TalkLine.SceneKey, Line);
	}

	TalkLine.Hash = FString::FromInt(GetTypeHash(TalkLine.SourceText));
	if (TextHashes.Contains(TalkLine.Hash))
	{
		int32 NumberOccurrences = CountOccurrences(TextHashes, TalkLine.Hash);
		TalkLine.DifferentAudioHash = FString::FromInt(
			GetTypeHash(TalkLine.SourceText + FString::FromInt(NumberOccurrences)));
	}

	TextHashes.Add(TalkLine.Hash);
	TalkLines->Lines.Add(TalkLine);
}

int32 UDataExtractor::CountOccurrences(const TArray<FString>& Array, FString Element)
{
	int32 Count = 0;
	for (FString Item : Array)
	{
		if (Item == Element)
		{
			Count++;
		}
	}
	return Count;
}

FString UDataExtractor::ExtractText(const FString& Key, FString& Line)
{
	FString SceneKeyCode = FScriptLineHandler::ApplySceneKeyIfNoSceneAlready(SceneKey, Key);
	FString TextString = Line.TrimStartAndEnd();
	return TextString;
}

FSceneKeyCode UDataExtractor::ExtractSameAsCode(FString& Line)
{
	FString EqualToKey = GetEqualToElement(Line);
	return FScriptLineHandler::ApplySceneKeyIfNoSceneAlready(SceneKey, EqualToKey);
}

void UDataExtractor::CreateDecision(const FString& Key, FString Line)
{
	FSceneKeyCode ParentKey = FScriptLineHandler::GetParentSceneKey(Key, SceneKey);
	if (!ActionsDataAsset->SceneData[ParentKey.Scene].DecisionsDataMap.Contains(ParentKey.Code))
	{
		FDecisionLines NewDecisionLines = FDecisionLines(ParentKey);
		ActionsDataAsset->SceneData[ParentKey.Scene].DecisionsDataMap.Add(ParentKey.Code, NewDecisionLines);
	}

	FDecisionLines* DecisionLines = &ActionsDataAsset->SceneData[ParentKey.Scene].DecisionsDataMap[ParentKey.Code];
	LastActionParentKey = ParentKey;
	DecisionLines->Character = SpeakerName;

	FDecisionLine DecisionLine = FDecisionLine(FScriptLineHandler::ApplySceneKeyIfNoSceneAlready(SceneKey, Key));
	TArray<FSceneKeyCode> NextActionNames;
	TArray<FInlineActionData> NextInlineActions;
	GetNextActions(Line, DecisionLine.NextKeys, DecisionLine.NextInlineActionDatas);

	DecisionLine.Condition = GetInlineConditionData(Line);
	DecisionLine.Modifiers = GetModifiers(Line);

	if (Line.Contains(FCodeSymbols::SameAsCode))
	{
		FSceneKeyCode ToCopy = ExtractSameAsCode(Line);
		CodesToUpdate.Add(DecisionLine.SceneKey, ToCopy);
	}
	else
	{
		DecisionLine.SourceText = ExtractText(DecisionLine.SceneKey, Line);
		DecisionLine.Hash = FString::FromInt(GetTypeHash(DecisionLine.SourceText));
	}

	DecisionLines->Lines.Add(DecisionLine);
}

void UDataExtractor::AddActionValue(FString& Line)
{
	TArray<FActionValue> Modifiers = GetModifiers(Line);
	if (LastActionParentKey.Code.Contains(FCodeSymbols::DecisionCode))
	{
		FDecisionLines& Decision = ActionsDataAsset->SceneData[LastActionParentKey.Scene].DecisionsDataMap[
			LastActionParentKey.Code];
		Decision.Modifiers = Modifiers;
	}
	else if (LastActionParentKey.Code.Contains(FCodeSymbols::TalkCode))
	{
		FTalkLines& TalkLines = ActionsDataAsset->SceneData[LastActionParentKey.Scene].TalkDataMap[LastActionParentKey.
			Code];
		TalkLines.Modifiers = Modifiers;
	}
}

void UDataExtractor::CreateAction(FString Key, FString Line)
{
	const FSceneKeyCode ParentKey = FScriptLineHandler::GetParentSceneKey(Key, SceneKey);
	if (!ActionsDataAsset->SceneData[ParentKey.Scene].ActionsDataMap.Contains(ParentKey.Code))
	{
		FActionData NewActionData = FActionData(FSceneKeyCode(SceneKey, ParentKey));
		ActionsDataAsset->SceneData[ParentKey.Scene].ActionsDataMap.Add(ParentKey.Code, NewActionData);
	}

	FActionData* ActionData = &ActionsDataAsset->SceneData[ParentKey.Scene].ActionsDataMap[ParentKey.Code];

	TArray<FSceneKeyCode> NextActionNames;
	TArray<FInlineActionData> NextInlineActions;
	GetNextActions(Line, NextActionNames, NextInlineActions);
	ActionData->NextKeys = NextActionNames;
	ActionData->NextInlineActionDatas = NextInlineActions;

	TArray<FSceneKeyCode> ParallelActionNames;
	TArray<FInlineActionData> ParallelInlineActions;
	GetParallelActions(Line, ParallelActionNames, ParallelInlineActions);
	ActionData->ParallelKeys = ParallelActionNames;
	ActionData->ParallelInlineActionDatas = ParallelInlineActions;

	TArray<FActionValue> Modifiers = GetModifiers(Line);
	ActionData->Data.Values = Modifiers;

	FString CodeName = Line.TrimStartAndEnd();
	ActionData->Data.CodeName = CodeName;
}

void UDataExtractor::CreateConditional(const FString& Key, FString Line)
{
	const FSceneKeyCode ParentKey = FScriptLineHandler::GetParentSceneKey(Key, SceneKey);
	if (!ActionsDataAsset->SceneData[ParentKey.Scene].ConditionsDataMap.Contains(ParentKey.Code))
	{
		FConditionGroup NewConditionGroup = FConditionGroup(ParentKey);
		ActionsDataAsset->SceneData[ParentKey.Scene].ConditionsDataMap.Add(ParentKey.Code, NewConditionGroup);
	}

	FConditionGroup* ConditionGroup = &ActionsDataAsset->SceneData[ParentKey.Scene].ConditionsDataMap[ParentKey.Code];
	FConditionData ConditionData = FConditionData(FSceneKeyCode(SceneKey, Key));
	ConditionData.InlineConditionData = GetInlineConditionData(Line);

	TArray<FString> ActionStrings = GetSeparatedActionStrings(Line);
	for (FString& ActionString : ActionStrings)
	{
		ActionString.TrimStartAndEndInline();

		if (ActionString.Len() < 2)
		{
			continue;
		}

		if (FChar::IsDigit(ActionString[1]))
		{
			FSceneKeyCode NextKey = FScriptLineHandler::GetSceneKeyCode(SceneKey, ActionString);
			FSceneKeyCode KeyCode = FScriptLineHandler::ApplySceneKeyIfNoSceneAlready(SceneKey, NextKey);
			ConditionData.ResultKeys.Add(KeyCode);
		}
		else
		{
			//
		}
	}
	ConditionGroup->Conditions.Add(ConditionData);
}

FInlineConditionData UDataExtractor::GetInlineConditionData(FString& Line)
{
	FInlineConditionData InlineConditionData;
	if (Line.Contains(FCodeSymbols::ConditionTrueCode))
	{
		TArray<FSceneKeyCode> Actions;
		TArray<FInlineActionData> InlineActions;
		GetActionsWithCode(Line, FCodeSymbols::ConditionTrueCode, Actions, InlineActions);
		AddActionsOnEmptyInline(Actions, InlineActions);
		InlineConditionData.TrueConditionsActionData = InlineActions;
	}

	if (Line.Contains(FCodeSymbols::ConditionFalseCode))
	{
		TArray<FSceneKeyCode> Actions;
		TArray<FInlineActionData> InlineActions;
		GetActionsWithCode(Line, FCodeSymbols::ConditionFalseCode, Actions, InlineActions);
		AddActionsOnEmptyInline(Actions, InlineActions);
		InlineConditionData.FalseConditionsActionData = InlineActions;
	}

	return InlineConditionData;
}

void UDataExtractor::AddActionsOnEmptyInline(TArray<FSceneKeyCode>& Actions, TArray<FInlineActionData>& InlineActions)
{
	if (InlineActions.IsEmpty())
	{
		for (FSceneKeyCode Action : Actions)
		{
			FInlineActionData InlineActionData;
			FActionValue ActionValue;
			ActionValue.Value = Action;
			InlineActionData.Values.Add(ActionValue);
			InlineActions.Add(InlineActionData);
		}
	}
}

FString UDataExtractor::GetEqualToElement(FString& Line)
{
	return FScriptLineHandler::ExtractElementBetween(Line, FCodeSymbols::SameAsCode);
}

FString UDataExtractor::GetActorComment(FString& Line)
{
	return FScriptLineHandler::ExtractElementBetween(Line, FCodeSymbols::ActorCommentCode);
}

FInlineActionData UDataExtractor::GetInlineActionData(FString& Line)
{
	Line.TrimStartAndEndInline();

	FInlineActionData InlineActionData;
	int32 Index = Line.Find(FCodeSymbols::ActionValuesStart);
	FString AllActionValues;
	if (Index != INDEX_NONE)
	{
		InlineActionData.CodeName = Line.Left(Index);
		InlineActionData.CodeName.TrimStartAndEndInline();
		AllActionValues = FScriptLineHandler::ExtractElementBetween(Line,
		                                                            FCodeSymbols::ActionValuesStart,
		                                                            FCodeSymbols::ActionValuesEnd);
	}
	else
	{
		Index = Line.Find(FCodeSymbols::ActionDefinitionValueCode);
		if (Index != INDEX_NONE)
		{
			InlineActionData.CodeName = Line.Left(Index);
			InlineActionData.CodeName.TrimStartAndEndInline();
			AllActionValues = Line.RightChop(Index + 1);
		}
		else
		{
			InlineActionData.CodeName = Line;
			//AllActionValues = Line;
		}
	}

	TArray<FString> Elements;
	AllActionValues.ParseIntoArray(Elements, *FCodeSymbols::ActionValueSeparation, true);

	TArray<FActionValue> ActionValues;
	for (FString& Element : Elements)
	{
		Element.TrimStartAndEndInline();
		FActionValue ActionValue = GetActionValue(Element);
		ActionValues.Add(ActionValue);
	}

	InlineActionData.Values = ActionValues;

	return InlineActionData;
}

TArray<FActionValue> UDataExtractor::GetModifiers(FString& Line)
{
	Line.TrimStartAndEndInline();

	FString ElementsString = FScriptLineHandler::ExtractElementBetween(Line,
	                                                                   FCodeSymbols::ActionValuesStart,
	                                                                   FCodeSymbols::ActionValuesEnd);
	TArray<FString> Elements;
	ElementsString.ParseIntoArray(Elements, *FCodeSymbols::ActionValueSeparation, true);

	TArray<FActionValue> ModifierInlineActions;
	for (FString Element : Elements)
	{
		FActionValue ActionValue = GetActionValue(Element);
		ModifierInlineActions.Add(ActionValue);
	}

	return ModifierInlineActions;
}

FActionValue UDataExtractor::GetActionValue(FString& Line)
{
	Line.TrimStartAndEndInline();

	FActionValue ActionValue;
	int32 Index = Line.Find(FCodeSymbols::ActionValuesStart);
	if (Index != INDEX_NONE)
	{
		ActionValue.CodeName = Line.Left(Index);
		ActionValue.CodeName.TrimStartAndEndInline();
		ActionValue.Value = FScriptLineHandler::ExtractElementBetween(Line,
		                                                              FCodeSymbols::ActionValuesStart,
		                                                              FCodeSymbols::ActionValuesEnd);
	}
	else
	{
		Index = Line.Find(FCodeSymbols::ActionDefinitionValueCode);
		if (Index != INDEX_NONE)
		{
			ActionValue.CodeName = Line.Left(Index);
			ActionValue.CodeName.TrimStartAndEndInline();
			ActionValue.Value = Line.RightChop(Index + 1);
			ActionValue.Value.TrimStartAndEndInline();
		}
		else
		{
			ActionValue.Value = Line;
		}
	}

	if (ActionValue.Value.IsValidIndex(1) && FChar::IsDigit(ActionValue.Value[1]) && !FChar::IsDigit(
		ActionValue.Value[0]) && ActionValue.Value[0] != 'S')
	{
		FSceneKeyCode Key = FScriptLineHandler::GetSceneKeyCode(SceneKey, ActionValue.Value);
		ActionValue.Value = Key;
		if (ActionValue.CodeName.IsEmpty())
		{
			ActionValue.CodeName = "SceneKeyCode";
		}
	}

	return ActionValue;
}

void UDataExtractor::GetNextActions(FString& Line, ::TArray<FSceneKeyCode>& Actions,
                                    TArray<FInlineActionData>& InlineActions)
{
	GetActionsWithCode(Line, FCodeSymbols::NextCode, Actions, InlineActions);
}

void UDataExtractor::GetParallelActions(FString& Line, TArray<FSceneKeyCode>& Actions,
                                        TArray<FInlineActionData>& InlineActions)
{
	GetActionsWithCode(Line, FCodeSymbols::ParallelCode, Actions, InlineActions);
}

void UDataExtractor::GetActionsWithCode(FString& Line, const FString& Code, TArray<FSceneKeyCode>& Actions,
                                        TArray<FInlineActionData>& InlineActions)
{
	if (!Line.Contains(Code))
	{
		return;
	}

	const FString ActionsString = FScriptLineHandler::ExtractElementBetween(Line, Code);
	TArray<FString> ActionStrings = GetSeparatedActionStrings(ActionsString);

	for (FString& ActionString : ActionStrings)
	{
		ActionString.TrimStartAndEndInline();

		if (ActionString.Len() < 2)
		{
			continue;
		}

		if (FChar::IsDigit(ActionString[1]))
		{
			FSceneKeyCode Key = FScriptLineHandler::GetSceneKeyCode(SceneKey, ActionString);
			Actions.Add(Key);
		}
		else
		{
			FInlineActionData InlineAction = GetInlineActionData(ActionString);
			if (!InlineAction.IsEmpty())
			{
				InlineActions.Add(InlineAction);
			}
		}
	}
}

TArray<FString> UDataExtractor::GetSeparatedActionStrings(FString ActionsString)
{
	TArray<FString> ActionStrings;
	//ActionsString.ParseIntoArray(ActionStrings, *FCodeSymbols::ActionValueSeparation, true);

	int32 BracketsCount = 0;
	FString CurrentAction;

	for (TCHAR Char : ActionsString)
	{
		if (Char == FCodeSymbols::ActionValuesStart[0])
		{
			BracketsCount++;
		}
		else if (Char == FCodeSymbols::ActionValuesEnd[0])
		{
			BracketsCount--;
		}

		if (Char == FCodeSymbols::ActionValueSeparation[0] && BracketsCount == 0)
		{
			ActionStrings.Add(CurrentAction);
			CurrentAction.Empty();
		}
		else
		{
			CurrentAction.AppendChar(Char);
		}
	}

	// Add the last action if there is any pending
	if (!CurrentAction.IsEmpty())
	{
		ActionStrings.Add(CurrentAction);
	}

	return ActionStrings;
}

FText UDataExtractor::GetTextReferenceFromTable(const FString& Key, const UStringTable* StringTable,
                                                const FString& TextValue)
{
	if (TextValue.IsEmpty())
	{
		return FText();
	}
	FText Text = FindTextFromString(StringTable, TextValue);
	if (Text.IsEmpty())
	{
		StringTable->GetMutableStringTable()->SetSourceString(Key, TextValue);
		Text = FText::FromStringTable(StringTable->GetStringTableId(), Key);
	}

	return Text;
}

FText UDataExtractor::FindTextFromString(const UStringTable* StringTable, const FString& Value)
{
	if (StringTable)
	{
		TArray<FString> Keys = UKismetStringTableLibrary::GetKeysFromStringTable(StringTable->GetStringTableId());
		for (const FString& Key : Keys)
		{
			FString EntryValue;
			StringTable->GetStringTable()->GetSourceString(Key, EntryValue);
			if (EntryValue == Value)
			{
				return FText::FromStringTable(StringTable->GetStringTableId(), Key);
			}
		}
	}

	return FText();
}