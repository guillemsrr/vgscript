// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TextDecisionCopy.h"

#include "VgScript/Data/KeyCode.h"
#include "VgScript/Data/SceneKeyCode.h"

#include "UObject/Object.h"
#include "CodeEnumerator.generated.h"

class UGUIDHandler;
class UVgScriptGUIDDataAsset;
/**
 * 
 */
UCLASS()
class VGSCRIPTEDITOR_API UCodeEnumerator : public UObject
{
	GENERATED_BODY()

public:
	void UpdateCodeEnumerations(TArray<FString>& Script);

	FORCEINLINE TArray<FString> GetChanges()
	{
		return Changes;
	}

private:
	FKeyCode SceneKey;

	int32 ActionsCounter;
	int32 DecisionsCounter;
	int32 ConditionsCounter;
	int32 TalksCounter;
	int32 SubCodeCounter;

	enum ELineType
	{
		Scene,
		Action,
		Decision,
		Condition,
		Talk,
		Comment,
		Alert,
		Person
	};

	ELineType LastLineType;

	UPROPERTY(Transient)
	TMap<FSceneKeyCode, FSceneKeyCode> ModifiedScriptCodesMap;
	UPROPERTY(Transient)
	TMap<FString, FString> SceneModifications;

	UPROPERTY(Transient)
	TMap<FKeyCode, FTextDecisionCopy> DecisionsTalkTextsMap;
	UPROPERTY(Transient)
	TMap<FSceneKeyCode, FString> DecisionsTextsMap;

	TArray<FString> Changes;

	void UpdateSceneNumbers(TArray<FString>& Script);
	void UpdateLines(TArray<FString>& Script);
	void UpdateEnumerations(FString& Line, ELineType& CurrentLineType);
	void AddNewLineAtStart(FString& Line);
	void AddNewLineAtEnd(FString& Line);

	void UpdateSceneCodeEnumeration(FString& Line);
	void AddModifiedCode(const FString& OriginalCode, const FString& ModifiedCode);
	void UpdateActionCodeEnumeration(FString& Line);
	void UpdateDualNumberCodeElement(FString& Line, const FString& InitialCode, int32& Counter);
	void UpdateTalkCodeEnumeration(FString& Line);
	void UpdateDecisionCodeEnumeration(FString& Line);
	void UpdateWithCodeAndCounter(FString& Line, FString Code, int32& Counter);
	void UpdateConditionCodeEnumeration(FString& Line);
	void SubstitutePossiblyModifiedCode(FString& Line);
	void SubstituteModifiedSceneCodes(FString& Line);

	bool IsIndexOutOfBounds(TArray<TTuple<int32, int32>> ChangedLineSections, int32 ChangeIndex);
	bool IsIndexAvailable(FString& Line, const TArray<TTuple<int32, int32>>& ChangedLineSections, int32 ChangeIndex,
	                      int32 CodeLength);
	int32 GetCodeNumber(FString Line, FString NumberCodeStart, FString NumberCodeEnd, int32& StartIndex, int32& Length,
	                    FString& OriginalNumberString);

	int32 CountScenes(TArray<FString>& Script);
	void ReplaceModifications(TArray<FString>& Script);
	void ExtractDecisionTexts(TArray<FString>& Script);
	void ApplyDecisionTexts(TArray<FString>& Script);
	void ApplyEqualDecisionTexts(TArray<FString>& Script);

	void AddLineChange(FString Before, FString After);

	FString FindFirstCodeFromIndex(FString Line, int32 Index, const FString& Code);

public:
	UPROPERTY(Transient)
	TObjectPtr<UGUIDHandler> GUIDHandler;

private:
	void UpdateMissingGUID(TArray<FString> Array);
};