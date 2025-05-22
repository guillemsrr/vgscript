// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "VgScript/Data/KeyCode.h"
#include "VgScript/Data/SceneKeyCode.h"
#include "VgScript/Data/Structs/Action/InlineActionData.h"
#include "VgScript/Data/Structs/Condition/InlineConditionData.h"

#include "UObject/Object.h"
#include "DataExtractor.generated.h"

class UActionDataAsset;

/**
 * 
 */
UCLASS()
class VGSCRIPTEDITOR_API UDataExtractor : public UObject
{
	GENERATED_BODY()

public:
	void SetActionsData(UActionDataAsset* InActionsDataAsset);
	void ExtractLineData(FString Line);

	//for references of subsequent actions
	void UpdateUnfinishedActions();

private:
	FKeyCode SceneKey;

	FName SpeakerName;
	FName DirectedAtName;

	FSceneKeyCode LastActionParentKey;

	UPROPERTY(Transient)
	TObjectPtr<UActionDataAsset> ActionsDataAsset;

	UPROPERTY(Transient)
	TMap<FSceneKeyCode, FSceneKeyCode> CodesToUpdate;
	UPROPERTY(Transient)
	TArray<FString> TextHashes;

	void CreateScene(const FString& Key, FString& Line);
	void CreateCharacter(const FString& Line);
	TArray<FActionValue> GetModifiers(FString& Line);
	void CreateText(const FString& Key, FString& Line);
	FSceneKeyCode ExtractSameAsCode(FString& Line);
	FString ExtractText(const FString& Key, FString& Line);
	FString GetEqualToElement(FString& Line);

	void CreateDecision(const FString& Key, FString Line);
	void AddActionValue(FString& Line);
	void CreateAction(FString Key, FString Line);
	FInlineConditionData GetInlineConditionData(FString& Line);
	void CreateConditional(const FString& Key, FString Line);

	void GetNextActions(FString& Line, TArray<FSceneKeyCode>& Actions, TArray<FInlineActionData>& InlineActions);
	void GetParallelActions(FString& Line, TArray<FSceneKeyCode>& Actions, TArray<FInlineActionData>& InlineActions);
	TArray<FString> GetSeparatedActionStrings(FString ActionsString);
	void GetActionsWithCode(FString& Line, const FString& Code, TArray<FSceneKeyCode>& Actions,
	                        TArray<FInlineActionData>& InlineActions);

	void AddActionsOnEmptyInline(TArray<FSceneKeyCode>& Actions, TArray<FInlineActionData>& InlineActions);

	//currently unused
	FText GetTextReferenceFromTable(const FString& Key, const UStringTable* StringTable, const FString& TextValue);
	FText FindTextFromString(const UStringTable* StringTable, const FString& Value);

	FString GetActorComment(FString& Line);
	FInlineActionData GetInlineActionData(FString& Line);
	FActionValue GetActionValue(FString& Line);

	int32 CountOccurrences(const TArray<FString>& Array, FString Element);
};