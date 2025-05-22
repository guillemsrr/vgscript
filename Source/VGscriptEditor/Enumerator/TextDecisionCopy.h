// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "TextDecisionCopy.generated.h"

USTRUCT()
struct VGSCRIPTEDITOR_API FTextDecisionCopy
{
	GENERATED_BODY()
	
	FTextDecisionCopy() = default;

	FString DecisionKey;
	FString Text;
};