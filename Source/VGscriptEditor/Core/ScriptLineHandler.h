// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "CodeSymbols.h"

#include "VgScript/Data/KeyCode.h"
#include "VgScript/Data/SceneKeyCode.h"

/**
 * 
 */
class VGSCRIPTEDITOR_API FScriptLineHandler
{
public:
	static FSceneKeyCode GetSceneKeyCode(const FKeyCode& SceneKeyCode, const FString& Key);

	static FString ExtractKey(FString& Line);
	static FString GetSceneKey(const FString& Key);
	static FSceneKeyCode GetParentSceneKey(FString Key, FKeyCode SceneKey);
	static FString ApplySceneKey(FKeyCode& SceneKey, FString Key);
	static void ApplySceneKeyInline(FString SceneKey, FString& Key);
	static FString ApplySceneKeyIfNoSceneAlready(FKeyCode& SceneKey, FString Key);
	static void ApplySceneKeyIfNoSceneAlreadyInline(FString SceneKey, FString& Key);

	static FString ExtractElementBetween(FString& Line, const FString StartCode,
	                                     const FString EndCode = FCodeSymbols::ElementEndCode,
	                                     const ESearchDir::Type EndSearchDir = ESearchDir::FromStart);
	static FString GetStringBetweenCodes(const FString& Line, const FString& StartCode, const FString& EndCode,
	                                     const ESearchDir::Type EndSearchDir = ESearchDir::FromStart);
};