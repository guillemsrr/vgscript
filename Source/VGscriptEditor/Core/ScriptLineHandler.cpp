// Copyright (c) Guillem Serra. All Rights Reserved.

#include "ScriptLineHandler.h"
#include "CodeSymbols.h"
#include "VgScript/Data/KeyCode.h"
#include "Kismet/KismetStringLibrary.h"

FSceneKeyCode FScriptLineHandler::GetSceneKeyCode(const FKeyCode& SceneKeyCode, const FString& Key)
{
	int32 SceneIndex = Key.Find(FCodeSymbols::SceneCode);
	if (SceneIndex == INDEX_NONE)
	{
		return FSceneKeyCode(SceneKeyCode, Key);
	}

	int32 SeparationIndex = Key.Find(FCodeSymbols::CodeSeparation);
	if (SeparationIndex == INDEX_NONE)
	{
		return FSceneKeyCode(SceneKeyCode, Key);
	}

	return FSceneKeyCode(Key);
}

FString FScriptLineHandler::ExtractKey(FString& Line)
{
	int32 StartIndex = Line.Find(FCodeSymbols::CodeStart, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	int32 EndIndex = Line.Find(FCodeSymbols::CodeEnd, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	FString Key = Line.Mid(StartIndex + 1, EndIndex - StartIndex - 1);

	Line.RightChopInline(EndIndex + 1);
	Line.TrimStartInline();
	Line.TrimEndInline();

	return Key;
}

FString FScriptLineHandler::GetSceneKey(const FString& Key)
{
	FString SceneKey = GetStringBetweenCodes(Key, FCodeSymbols::CodeStart, FCodeSymbols::CodeEnd);
	SceneKey.TrimStartAndEndInline();

	return SceneKey;
}

FSceneKeyCode FScriptLineHandler::GetParentSceneKey(FString Key, FKeyCode SceneKey)
{
	TArray<FString> Keys;
	Key.ParseIntoArray(Keys, *FCodeSymbols::CodeSeparation, true);

	if (Keys.IsEmpty())
	{
		return FSceneKeyCode(SceneKey);
	}

	for (FString& String : Keys)
	{
		String.ReplaceInline(*FCodeSymbols::CodeSeparation, TEXT(""));
	}

	if (Key[0] == FCodeSymbols::SceneCode[0])
	{
		return FSceneKeyCode(Keys[0], Keys[1]);
	}

	return FSceneKeyCode(SceneKey, Keys[0]);
}

FString FScriptLineHandler::ApplySceneKey(FKeyCode& SceneKey, FString Key)
{
	return SceneKey.ToString() + FCodeSymbols::CodeSeparation + Key;
}

void FScriptLineHandler::ApplySceneKeyInline(FString SceneKey, FString& Key)
{
	Key = SceneKey + FCodeSymbols::CodeSeparation + Key;
}

FString FScriptLineHandler::ApplySceneKeyIfNoSceneAlready(FKeyCode& SceneKey, FString Key)
{
	Key.TrimStartAndEndInline();

	int32 SceneIndex = Key.Find(FCodeSymbols::SceneCode);
	if (SceneIndex == -1)
	{
		return ApplySceneKey(SceneKey, Key);
	}

	return Key;
}

void FScriptLineHandler::ApplySceneKeyIfNoSceneAlreadyInline(FString SceneKey, FString& Key)
{
	int32 SceneIndex = Key.Find(FCodeSymbols::SceneCode);
	if (SceneIndex == -1)
	{
		ApplySceneKeyInline(SceneKey, Key);
	}
}

FString FScriptLineHandler::ExtractElementBetween(FString& Line, const FString StartCode, const FString EndCode,
                                                  const ESearchDir::Type EndSearchDir)
{
	FString Element = GetStringBetweenCodes(Line, StartCode, EndCode, EndSearchDir);
	UKismetStringLibrary::ReplaceInline(Line, StartCode + Element + EndCode, "");
	Element.TrimStartInline();
	Element.TrimEndInline();
	return Element;
}

FString FScriptLineHandler::GetStringBetweenCodes(const FString& Line, const FString& StartCode, const FString& EndCode,
                                                  const ESearchDir::Type EndSearchDir)
{
	int32 StartIndex = Line.Find(StartCode) + StartCode.Len() - 1;
	int32 EndIndex = INDEX_NONE;
	if (EndSearchDir == ESearchDir::FromStart)
	{
		EndIndex = Line.Find(EndCode, ESearchCase::IgnoreCase, ESearchDir::FromStart, StartIndex + 1);
	}
	else if (EndSearchDir == ESearchDir::FromEnd)
	{
		EndIndex = Line.Find(EndCode, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	}

	if (StartIndex == INDEX_NONE || EndIndex == INDEX_NONE)
	{
		return FString();
	}

	int32 Length = EndIndex - StartIndex - 1;
	return Line.Mid(StartIndex + 1, Length);
}