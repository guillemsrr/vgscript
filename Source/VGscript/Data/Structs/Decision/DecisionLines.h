// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "DecisionLine.h"
#include "VgScript/Data/Structs/Base/ActionDataBase.h"
#include "DecisionLines.generated.h"

USTRUCT()
struct VGSCRIPT_API FDecisionLines : public FActionDataBase
{
	GENERATED_BODY()

	using FActionDataBase::FActionDataBase;

	UPROPERTY(VisibleAnywhere, meta = (TitleProperty = "LineKey", ForceInlineRow))
	TArray<FDecisionLine> Lines;

	UPROPERTY(VisibleDefaultsOnly)
	TArray<FActionValue> Modifiers;

	UPROPERTY(VisibleDefaultsOnly)
	FName Character;

	int32 GetLineIndex(const FSceneKeyCode& InSceneKeyCode);
};