// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "VgScript/Data/SceneKeyCode.h"

#include "ActionDataBase.generated.h"

USTRUCT()
struct VGSCRIPT_API FActionDataBase
{
	GENERATED_BODY()

	FActionDataBase() = default;
	virtual ~FActionDataBase() = default;

	explicit FActionDataBase(const FSceneKeyCode& InSceneKey)
	{
		this->SceneKeyCode = InSceneKey;
	}

	UPROPERTY(VisibleAnywhere)
	FSceneKeyCode SceneKeyCode;
};