// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "TalkLine.h"
#include "VgScript/Data/Structs/Base/ActionDataBase.h"
#include "TalkLines.generated.h"

USTRUCT()
struct VGSCRIPT_API FTalkLines: public FActionDataBase
{
	GENERATED_BODY()

	FTalkLines() = default;

	using FActionDataBase::FActionDataBase;

	UPROPERTY(VisibleAnywhere)
	FName Talker;
	UPROPERTY(VisibleAnywhere)
	FName DirectedTo;
	
	UPROPERTY(VisibleAnywhere, meta = (TitleProperty = "LineKey", ForceInlineRow))
	TArray<FTalkLine> Lines;

	UPROPERTY(VisibleDefaultsOnly)
	TArray<FActionValue> Modifiers;
	
	int32 GetLineIndex(const FSceneKeyCode& InSceneKeyCode);
};

//TODO: just add SceneKeyCode Scene, Code, SubIndex
inline int32 FTalkLines::GetLineIndex(const FSceneKeyCode& InSceneKeyCode)
{
	int32 Index = 0;
	for (const FTalkLine& Line : Lines)
	{
		if (Line.SceneKey == InSceneKeyCode)
		{
			return Index;
		}

		Index++;
	}

	return INDEX_NONE;
}