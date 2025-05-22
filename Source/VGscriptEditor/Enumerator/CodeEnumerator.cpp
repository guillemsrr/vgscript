// Copyright (c) Guillem Serra. All Rights Reserved.

#include "CodeEnumerator.h"

#include "VgScriptEditor/Core/CodeSymbols.h"
#include "VgScriptEditor/Core/ScriptLineHandler.h"
#include "VgScriptEditor/GUID/GUIDHandler.h"

void UCodeEnumerator::UpdateCodeEnumerations(TArray<FString>& Script)
{
	ModifiedScriptCodesMap.Empty();
	DecisionsTalkTextsMap.Empty();
	DecisionsTextsMap.Empty();
	LastLineType = Comment;

	UpdateSceneNumbers(Script);
	for (FString& Line : Script)
	{
		SubstituteModifiedSceneCodes(Line);
	}

	GUIDHandler->HandleSceneReplacements(SceneModifications);
	ModifiedScriptCodesMap.Empty();

	UpdateLines(Script);
	ReplaceModifications(Script);

	ExtractDecisionTexts(Script);
	ApplyDecisionTexts(Script);
	ApplyEqualDecisionTexts(Script);
	GUIDHandler->HandleGUIDReplacements(ModifiedScriptCodesMap);

	GUIDHandler->CleanRepeated();
	UpdateMissingGUID(Script);
}

void UCodeEnumerator::UpdateSceneNumbers(TArray<FString>& Script)
{
	int32 ScenesCounter = CountScenes(Script);

	int32 NumberLines = Script.Num();
	for (int i = NumberLines - 1; i >= 0; i--)
	{
		FString Line = Script[i];
		FString LineCopy = Line;
		FString Key = FScriptLineHandler::ExtractKey(LineCopy);

		if (Key.StartsWith(FCodeSymbols::SceneCode))
		{
			FString NumberString = Key.RightChop(1);
			int32 SceneNumber = FCString::Atoi(*NumberString);
			if (SceneNumber != ScenesCounter)
			{
				FString UpdatedSceneKey = FCodeSymbols::SceneCode + FString::FromInt(ScenesCounter);
				Script[i].ReplaceInline(*Key, *UpdatedSceneKey);

				SceneModifications.Add(Key, UpdatedSceneKey);

				for (FString& LineWithSceneReferences : Script)
				{
					if (LineWithSceneReferences.Contains(FCodeSymbols::CodeStart + FCodeSymbols::SceneCode))
					{
						continue;
					}

					int32 Index = 0;
					while ((Index = LineWithSceneReferences.Find(Key,
					                                             ESearchCase::CaseSensitive,
					                                             ESearchDir::FromStart,
					                                             Index)) != INDEX_NONE)
					{
						int32 IndexStart = Index;
						//add separation and code symbol
						Index += 2;

						while (LineWithSceneReferences.IsValidIndex(Index + 1) && TChar<TCHAR>::IsDigit(
							LineWithSceneReferences[Index + 1]))
						{
							Index++;
						}

						int32 Length = Index - IndexStart + Key.Len() + 1;
						FString Substring = LineWithSceneReferences.Mid(IndexStart, Length);
						FSceneKeyCode Original = Substring;
						FSceneKeyCode Updated = Original;
						Updated.Scene = UpdatedSceneKey;

						if (!ModifiedScriptCodesMap.Contains(Original))
						{
							ModifiedScriptCodesMap.Add(Original, Updated);
						}
					}
				}
			}

			ScenesCounter--;
		}
	}
}

void UCodeEnumerator::UpdateSceneCodeEnumeration(FString& Line)
{
	ActionsCounter = 0;
	ConditionsCounter = 0;
	DecisionsCounter = 0;
	TalksCounter = 0;

	SceneKey = FScriptLineHandler::GetSceneKey(Line);
}

void UCodeEnumerator::AddModifiedCode(const FString& OriginalCode, const FString& ModifiedCode)
{
	FSceneKeyCode SceneCode = FScriptLineHandler::GetSceneKeyCode(SceneKey, OriginalCode);
	if (ModifiedScriptCodesMap.Contains(SceneCode))
	{
		return;
	}
	ModifiedScriptCodesMap.Add(SceneCode, ModifiedCode);
}

void UCodeEnumerator::UpdateActionCodeEnumeration(FString& Line)
{
	UpdateWithCodeAndCounter(Line, FCodeSymbols::ActionCode, ActionsCounter);
}

void UCodeEnumerator::AddNewLineAtEnd(FString& Line)
{
	Line.Append(FCodeSymbols::EndLine);
}

void UCodeEnumerator::AddNewLineAtStart(FString& Line)
{
	Line.InsertAt(0, FCodeSymbols::EndLine);
}

void UCodeEnumerator::UpdateDualNumberCodeElement(FString& Line, const FString& InitialCode, int32& Counter)
{
	int32 StartIndex;
	int32 Length;
	FString NumberString;

	FString SelectedCodeEnd;
	SelectedCodeEnd = Line.Contains(FCodeSymbols::CodeSeparation)
		                  ? FCodeSymbols::CodeSeparation
		                  : FCodeSymbols::CodeEnd;
	int32 CodeNumber = GetCodeNumber(Line,
	                                 FCodeSymbols::CodeStart + InitialCode,
	                                 SelectedCodeEnd,
	                                 StartIndex,
	                                 Length,
	                                 NumberString);
	if (CodeNumber == INDEX_NONE)
	{
		return;
	}

	int32 SubStartIndex;
	int32 SubLength;
	FString SubNumberString;
	int32 SubNumber = GetCodeNumber(Line,
	                                FCodeSymbols::CodeSeparation,
	                                FCodeSymbols::CodeEnd,
	                                SubStartIndex,
	                                SubLength,
	                                SubNumberString);
	if (SubNumber == INDEX_NONE)
	{
		return;
	}

	if (SubLength < 0)
	{
		return;
	}

	if (SubNumber == 1)
	{
		SubCodeCounter = 1; //this means that "-1" need to be correct in the script.
		Counter++;
	}

	FString FinalNumberString = NumberString;
	if (CodeNumber != Counter)
	{
		Line.RemoveAt(StartIndex + 1, Length);

		FinalNumberString = FString::FromInt(Counter);
		Line.InsertAt(StartIndex + 1, FinalNumberString);

		AddModifiedCode(InitialCode + NumberString, InitialCode + FinalNumberString);
	}

	//again, in case the previous code changed the line indexes.
	SubNumber = GetCodeNumber(Line,
	                          FCodeSymbols::CodeSeparation,
	                          FCodeSymbols::CodeEnd,
	                          SubStartIndex,
	                          SubLength,
	                          SubNumberString);

	if (SubNumber != SubCodeCounter)
	{
		Line.RemoveAt(SubStartIndex + 1, SubLength);

		FString NewNumber = FString::FromInt(SubCodeCounter);
		Line.InsertAt(SubStartIndex + 1, NewNumber);

		AddModifiedCode(InitialCode + FinalNumberString + FCodeSymbols::CodeSeparation + SubNumberString,
		                InitialCode + FinalNumberString + FCodeSymbols::CodeSeparation + NewNumber);
	}

	SubCodeCounter++;
}

void UCodeEnumerator::UpdateTalkCodeEnumeration(FString& Line)
{
	UpdateDualNumberCodeElement(Line, FCodeSymbols::TalkCode, TalksCounter);
}

void UCodeEnumerator::UpdateDecisionCodeEnumeration(FString& Line)
{
	UpdateDualNumberCodeElement(Line, FCodeSymbols::DecisionCode, DecisionsCounter);
}

void UCodeEnumerator::UpdateWithCodeAndCounter(FString& Line, FString Code, int32& Counter)
{
	int32 StartIndex;
	int32 Length;
	FString NumberString;
	int32 CodeNumber = GetCodeNumber(Line,
	                                 FCodeSymbols::CodeStart + Code,
	                                 FCodeSymbols::CodeEnd,
	                                 StartIndex,
	                                 Length,
	                                 NumberString);

	Counter++;

	if (CodeNumber != Counter)
	{
		FString LineBefore = Line;
		Line.RemoveAt(StartIndex + 1, Length);

		FString NewNumber = FString::FromInt(Counter);
		Line.InsertAt(StartIndex + 1, NewNumber);

		AddLineChange(LineBefore, Line);

		AddModifiedCode(Code + NumberString, Code + NewNumber);
	}
}

void UCodeEnumerator::UpdateConditionCodeEnumeration(FString& Line)
{
	FString Code = FScriptLineHandler::GetStringBetweenCodes(Line, FCodeSymbols::CodeStart, FCodeSymbols::CodeEnd);
	if (Code.Contains(FCodeSymbols::CodeSeparation))
	{
		UpdateDualNumberCodeElement(Line, FCodeSymbols::ConditionCode, ConditionsCounter);
	}
	else
	{
		UpdateWithCodeAndCounter(Line, FCodeSymbols::ConditionCode, ConditionsCounter);
	}
}

int32 UCodeEnumerator::GetCodeNumber(FString Line, FString NumberCodeStart, FString NumberCodeEnd, int32& StartIndex,
                                     int32& Length, FString& OriginalNumberString)
{
	StartIndex = Line.Find(NumberCodeStart) + NumberCodeStart.Len() - 1;
	if (StartIndex == INDEX_NONE)
	{
		return INDEX_NONE;
	}
	int32 EndIndex = Line.Find(NumberCodeEnd);
	if (EndIndex == INDEX_NONE)
	{
		return INDEX_NONE;
	}
	Length = EndIndex - StartIndex - 1;
	OriginalNumberString = Line.Mid(StartIndex + 1, Length);
	if (OriginalNumberString.IsNumeric())
	{
		return FCString::Atoi(*OriginalNumberString);
	}
	else
	{
		return 0;
	}
}

void UCodeEnumerator::SubstitutePossiblyModifiedCode(FString& Line)
{
	TArray<TTuple<int32, int32>> ChangedLineSections;
	for (TTuple<FSceneKeyCode, FSceneKeyCode>& ModifiedCodeTuple : ModifiedScriptCodesMap)
	{
		FKeyCode ModifiedSceneCode = ModifiedCodeTuple.Key.Scene;
		FKeyCode Code = ModifiedCodeTuple.Key.Code;
		int32 CodeLength = Code.ToString().Len();
		int32 ChangeIndex = 0;

		while ((ChangeIndex = Line.Find(Code, ESearchCase::CaseSensitive, ESearchDir::FromStart, ChangeIndex)) !=
			INDEX_NONE)
		{
			if (IsIndexAvailable(Line, ChangedLineSections, ChangeIndex, CodeLength))
			{
				ChangeIndex += CodeLength;
				continue;
			}

			TCHAR PossileNextSceneSeparatorChar = Line[ChangeIndex - 1];
			FString PossibleSeparatorString = FString(1, &PossileNextSceneSeparatorChar);
			if (PossibleSeparatorString.Equals(FCodeSymbols::CodeSeparation))
			{
				FString Scene = FindFirstCodeFromIndex(Line, ChangeIndex - 1, FCodeSymbols::SceneCode);
				FString ModifiedSceneString = ModifiedSceneCode.ToString();
				if (!Scene.Equals(ModifiedSceneString, ESearchCase::Type::IgnoreCase))
				{
					ChangeIndex += CodeLength;
					continue;
				}
			}

			FString SubCode = ModifiedCodeTuple.Key.Scene + FCodeSymbols::CodeSeparation + ModifiedCodeTuple.Key.Code;
			if (ModifiedCodeTuple.Key.Scene != SceneKey && !Line.Contains(SubCode))
			{
				ChangeIndex += CodeLength;
				continue;
			}

			if (Code.Contains(FCodeSymbols::CodeSeparation))
			{
				int32 PossibleSeparatorIndex = Line.Find(FCodeSymbols::CodeSeparation,
				                                         ESearchCase::CaseSensitive,
				                                         ESearchDir::FromStart,
				                                         ChangeIndex);

				TCHAR CharacterAtIndex = Line[PossibleSeparatorIndex];
				FString StringAtIndex = FString(1, &CharacterAtIndex);
				if (!StringAtIndex.Equals(FCodeSymbols::CodeSeparation))
				{
					ChangeIndex += CodeLength;
					continue;
				}
			}

			FString LineBefore = Line;
			Line.RemoveAt(ChangeIndex, CodeLength);
			Line.InsertAt(ChangeIndex, ModifiedCodeTuple.Value);

			AddLineChange(LineBefore, Line);

			ChangedLineSections.Add(TTuple<int32, int32>(ChangeIndex, CodeLength));

			ChangeIndex += CodeLength;
		}
	}
}

void UCodeEnumerator::SubstituteModifiedSceneCodes(FString& Line)
{
	TArray<TTuple<int32, int32>> ChangedLineSections;
	for (TTuple<FSceneKeyCode, FSceneKeyCode>& ModifiedCodeTuple : ModifiedScriptCodesMap)
	{
		int32 CodeLength = ModifiedCodeTuple.Key.ToString().Len();
		int32 ChangeIndex = 0;

		while ((ChangeIndex = Line.Find(ModifiedCodeTuple.Key,
		                                ESearchCase::CaseSensitive,
		                                ESearchDir::FromStart,
		                                ChangeIndex)) != INDEX_NONE)
		{
			if (IsIndexAvailable(Line, ChangedLineSections, ChangeIndex, CodeLength))
			{
				ChangeIndex += CodeLength;
				continue;
			}

			FString LineBefore = Line;
			Line.RemoveAt(ChangeIndex, CodeLength);
			Line.InsertAt(ChangeIndex, ModifiedCodeTuple.Value);
			AddLineChange(LineBefore, Line);
			ChangedLineSections.Add(TTuple<int32, int32>(ChangeIndex, CodeLength));

			ChangeIndex += CodeLength;
		}
	}
}

bool UCodeEnumerator::IsIndexAvailable(FString& Line, const TArray<TTuple<int32, int32>>& ChangedLineSections,
                                       int32 ChangeIndex, int32 CodeLength)
{
	if (IsIndexOutOfBounds(ChangedLineSections, ChangeIndex))
	{
		return true;
	}

	int32 NextToCodeCharacterIndex = ChangeIndex - 1;
	if (Line.IsValidIndex(NextToCodeCharacterIndex) && Line[NextToCodeCharacterIndex] == FCodeSymbols::CodeStart[0])
	{
		return true;
	}

	NextToCodeCharacterIndex = ChangeIndex + CodeLength;
	if (Line.IsValidIndex(NextToCodeCharacterIndex) && FChar::IsDigit(Line[NextToCodeCharacterIndex]))
	{
		return true;
	}

	return false;
}

bool UCodeEnumerator::IsIndexOutOfBounds(TArray<TTuple<int32, int32>> ChangedLineSections, int32 ChangeIndex)
{
	for (TTuple<int32, int32> ChangedLineSection : ChangedLineSections)
	{
		int32 LowerBound = ChangedLineSection.Get<0>();
		int32 UpperBound = LowerBound + ChangedLineSection.Get<1>();

		if (ChangeIndex >= LowerBound && ChangeIndex <= UpperBound)
		{
			return true;
		}
	}

	return false;
}

int32 UCodeEnumerator::CountScenes(TArray<FString>& Script)
{
	int32 ScenesCounter = 0;
	for (FString Line : Script)
	{
		if (Line.StartsWith(FCodeSymbols::CodeStart + FCodeSymbols::SceneCode))
		{
			ScenesCounter++;
		}
	}

	return ScenesCounter;
}

void UCodeEnumerator::UpdateEnumerations(FString& Line, ELineType& CurrentLineType)
{
	FString LineCopy = Line;
	FString Key = FScriptLineHandler::ExtractKey(LineCopy);

	if (Line.IsEmpty())
	{
		CurrentLineType = Comment;
		//AddNewLineAtEnd(Line);
	}
	else
	{
		if (Key.StartsWith(FCodeSymbols::SceneCode))
		{
			CurrentLineType = Scene;
			UpdateSceneCodeEnumeration(Line);
		}
		else if (Key.StartsWith(FCodeSymbols::TalkCode))
		{
			CurrentLineType = Talk;
			UpdateTalkCodeEnumeration(Line);
		}
		else if (Key.StartsWith(FCodeSymbols::DecisionCode))
		{
			CurrentLineType = Decision;
			UpdateDecisionCodeEnumeration(Line);
		}
		else if (Key.StartsWith(FCodeSymbols::ActionCode))
		{
			CurrentLineType = Action;
			UpdateActionCodeEnumeration(Line);
		}
		else if (Key.StartsWith(FCodeSymbols::ConditionCode))
		{
			CurrentLineType = Condition;
			UpdateConditionCodeEnumeration(Line);
		}
		else if (Key.StartsWith(FCodeSymbols::AlertScriptComment))
		{
			CurrentLineType = Alert;
		}
		else if (Key.StartsWith(FCodeSymbols::PersonTalkingCode))
		{
			CurrentLineType = Person;
		}
		else if (Line.StartsWith(FCodeSymbols::ActionValuesStart))
		{
			CurrentLineType = LastLineType;
		}
		else
		{
			CurrentLineType = Comment;
		}
	}
}

void UCodeEnumerator::ReplaceModifications(TArray<FString>& Script)
{
	for (FString& Line : Script)
	{
		if (Line.Contains(FCodeSymbols::CodeStart + FCodeSymbols::SceneCode))
		{
			SceneKey = FScriptLineHandler::GetSceneKey(Line);
		}

		if (Line.IsEmpty() || !SceneKey)
		{
			continue;
		}

		SubstitutePossiblyModifiedCode(Line);
	}
}

void UCodeEnumerator::ExtractDecisionTexts(TArray<FString>& Script)
{
	for (FString& Line : Script)
	{
		if (Line.Contains(FCodeSymbols::CodeStart + FCodeSymbols::SceneCode))
		{
			SceneKey = FScriptLineHandler::GetSceneKey(Line);
		}

		if (!Line.Contains(FCodeSymbols::CodeStart + FCodeSymbols::DecisionCode))
		{
			continue;
		}

		if (Line.Contains(FCodeSymbols::SameAsCode))
		{
			continue;
		}

		FString LineCopy = Line;
		FString DecisionKey = FScriptLineHandler::ExtractKey(LineCopy);

		TArray<FKeyCode> NextTexts;
		FString ActionsString = FScriptLineHandler::ExtractElementBetween(LineCopy, FCodeSymbols::NextCode);
		TArray<FString> ActionStrings;
		ActionsString.ParseIntoArray(ActionStrings, *FCodeSymbols::ActionValueSeparation, true);

		for (FString& ActionString : ActionStrings)
		{
			ActionString.TrimStartAndEndInline();
			FSceneKeyCode Key = FScriptLineHandler::GetSceneKeyCode(SceneKey, ActionString);
			NextTexts.Add(Key);
		}

		if (NextTexts.IsEmpty())
		{
			continue;
		}

		FString TextAction = NextTexts[0].ToString();
		FScriptLineHandler::ApplySceneKeyIfNoSceneAlreadyInline(SceneKey, TextAction);

		FScriptLineHandler::ExtractElementBetween(LineCopy, FCodeSymbols::ConditionTrueCode);
		FScriptLineHandler::ExtractElementBetween(LineCopy, FCodeSymbols::ConditionFalseCode);
		FScriptLineHandler::ExtractElementBetween(LineCopy,
		                                          FCodeSymbols::ActionValuesStart,
		                                          FCodeSymbols::ActionValuesEnd);

		FString Text = LineCopy.TrimStartAndEnd();

		FTextDecisionCopy TextDecisionCopy;
		TextDecisionCopy.DecisionKey = DecisionKey;
		TextDecisionCopy.Text = Text;
		DecisionsTalkTextsMap.Add(TextAction, TextDecisionCopy);

		FScriptLineHandler::ApplySceneKeyIfNoSceneAlreadyInline(SceneKey, DecisionKey);

		DecisionsTextsMap.Add(DecisionKey, Text);
	}
}

void UCodeEnumerator::ApplyDecisionTexts(TArray<FString>& Script)
{
	for (FString& Line : Script)
	{
		if (Line.Contains(FCodeSymbols::CodeStart + FCodeSymbols::SceneCode))
		{
			SceneKey = FScriptLineHandler::GetSceneKey(Line);
			continue;
		}

		if (!Line.Contains(FCodeSymbols::CodeStart + FCodeSymbols::TalkCode))
		{
			continue;
		}

		FString LineBefore = Line;
		FString LineCopy = Line;
		FString TextKeyAndSub = FScriptLineHandler::ExtractKey(LineCopy);
		FString TextKey = FScriptLineHandler::GetParentSceneKey(TextKeyAndSub, SceneKey);
		int32 SubStartIndex;
		int32 SubLength;
		FString SubNumberString;

		int32 SubTalkNumber = GetCodeNumber(Line,
		                                    FCodeSymbols::CodeSeparation,
		                                    FCodeSymbols::CodeEnd,
		                                    SubStartIndex,
		                                    SubLength,
		                                    SubNumberString);
		if (SubTalkNumber != 1)
		{
			continue;
		}

		FScriptLineHandler::ApplySceneKeyIfNoSceneAlreadyInline(SceneKey, TextKey);

		if (!DecisionsTalkTextsMap.Contains(TextKey))
		{
			continue;
		}

		FTextDecisionCopy TextDecisionCopy = DecisionsTalkTextsMap[TextKey];
		FScriptLineHandler::ExtractElementBetween(Line, FCodeSymbols::SameAsCode, FCodeSymbols::ElementEndCode);
		FScriptLineHandler::ExtractElementBetween(Line,
		                                          FCodeSymbols::TextCode,
		                                          FCodeSymbols::TextCode,
		                                          ESearchDir::FromEnd);

		int32 StartIndex = Line.Find(FCodeSymbols::CodeEnd, ESearchCase::CaseSensitive, ESearchDir::FromStart);
		Line.InsertAt(StartIndex + 1, FCodeSymbols::TextCode + TextDecisionCopy.Text + FCodeSymbols::TextCode);
		Line.InsertAt(StartIndex + 1,
		              FCodeSymbols::SameAsCode + TextDecisionCopy.DecisionKey + FCodeSymbols::ElementEndCode);
		if (!LineBefore.Equals(Line))
		{
			AddLineChange(LineBefore, Line);
		}
	}
}

void UCodeEnumerator::ApplyEqualDecisionTexts(TArray<FString>& Script)
{
	for (FString& Line : Script)
	{
		if (Line.Contains(FCodeSymbols::CodeStart + FCodeSymbols::SceneCode))
		{
			SceneKey = FScriptLineHandler::GetSceneKey(Line);
			continue;
		}

		if (!Line.Contains(FCodeSymbols::CodeStart + FCodeSymbols::DecisionCode))
		{
			continue;
		}

		if (!Line.Contains(FCodeSymbols::SameAsCode))
		{
			continue;
		}

		FString LineBefore = Line;
		FString LineCopy = Line;
		FString EqualDecisionKey = FScriptLineHandler::ExtractElementBetween(
			LineCopy,
			FCodeSymbols::SameAsCode,
			FCodeSymbols::ElementEndCode);
		FScriptLineHandler::ApplySceneKeyIfNoSceneAlreadyInline(SceneKey, EqualDecisionKey);
		FScriptLineHandler::ExtractElementBetween(Line,
		                                          FCodeSymbols::TextCode,
		                                          FCodeSymbols::TextCode,
		                                          ESearchDir::FromEnd);

		if (!DecisionsTextsMap.Contains(EqualDecisionKey))
		{
			continue;
		}

		FString DecisionText = DecisionsTextsMap[EqualDecisionKey];
		int32 StartIndex = Line.Find(FCodeSymbols::ElementEndCode, ESearchCase::CaseSensitive, ESearchDir::FromStart);
		Line.InsertAt(StartIndex + 1, FCodeSymbols::TextCode + DecisionText + FCodeSymbols::TextCode);

		if (!LineBefore.Equals(Line))
		{
			AddLineChange(LineBefore, Line);
		}
	}
}

void UCodeEnumerator::AddLineChange(FString Before, FString After)
{
	Changes.Add(Before + " -------- > " + After);
}

FString UCodeEnumerator::FindFirstCodeFromIndex(FString Line, int32 Index, const FString& Code)
{
	if (Index <= 0 || Index >= Line.Len())
	{
		return "";
	}

	int32 CurrentIndex = Index - 1;
	FString NumberPart = "";

	while (CurrentIndex >= 0 && FChar::IsDigit(Line[CurrentIndex]))
	{
		NumberPart.InsertAt(0, Line[CurrentIndex]);
		CurrentIndex--;
	}

	// Check if we stopped at 'S'
	FString CharAsString(1, &Line[CurrentIndex]);
	if (CurrentIndex >= 0 && CharAsString == Code)
	{
		return Code + NumberPart; // Return full SXXX
	}

	return TEXT("");
}

void UCodeEnumerator::UpdateMissingGUID(TArray<FString> Script)
{
	for (FString& Line : Script)
	{
		if (Line.IsEmpty() || Line.StartsWith(FCodeSymbols::CommentLine))
		{
			continue;
		}

		FString LineCopy = Line;
		FString Key = FScriptLineHandler::ExtractKey(LineCopy);

		if (Key.StartsWith(FCodeSymbols::PersonTalkingCode))
		{
			continue;
		}

		if (Key.StartsWith(FCodeSymbols::SceneCode))
		{
			SceneKey = FScriptLineHandler::GetSceneKey(Line);
			continue;
		}

		Key.TrimStartAndEndInline();

		if (Key.IsEmpty())
		{
			continue;
		}

		if (Key.Contains("!"))
		{
			continue;
		}

		int32 SubCodeIndex = Key.Find(FCodeSymbols::CodeSeparation);
		if (SubCodeIndex == INDEX_NONE)
		{
			GUIDHandler->AddGUID(FSceneKeyCode(SceneKey, Key));
			continue;
		}

		Key = Key.Left(SubCodeIndex);
		FSceneKeyCode SubLineSceneKeyCode = FSceneKeyCode(SceneKey, Key);
		GUIDHandler->AddGUID(SubLineSceneKeyCode);
	}
}

void UCodeEnumerator::UpdateLines(TArray<FString>& Script)
{
	for (FString& Line : Script)
	{
		if (Line.IsEmpty() || Line.StartsWith(FCodeSymbols::CommentLine))
		{
			continue;
		}

		ELineType CurrentLineType;
		UpdateEnumerations(Line, CurrentLineType);

		if (LastLineType != Scene)
		{
			if (LastLineType != CurrentLineType)
			{
				AddNewLineAtStart(Line);
			}
			else if (Line.Contains("-1]"))
			{
				AddNewLineAtStart(Line);
			}
		}

		LastLineType = CurrentLineType;
	}
}