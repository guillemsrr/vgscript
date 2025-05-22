// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SceneData.h"
#include "Engine/DataAsset.h"
#include "ActionDataAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class VGSCRIPT_API UActionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = "Name", ForceInlineRow))
	TMap<FString, FSceneData> SceneData;

	FTalkLines* GetTalkLines(FSceneKeyCode Key);
	FTalkLine* GetTalkLine(const FSceneKeyCode& Key);
	FDecisionLines* GetDecisionLines(FSceneKeyCode Key);
	FDecisionLine* GetDecisionLine(FSceneKeyCode Key);
};