// Copyright (c) Noümen Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CodeGUIDPair.h"
#include "SceneGUIDData.h"

#include "VgScript/Data/SceneKeyCode.h"

#include "Engine/DataAsset.h"
#include "VgScriptGUIDDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class VGSCRIPT_API UVgScriptGUIDDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	FCodeGUIDPair* GetCodeGUIPair(const FSceneKeyCode& Code);

	TArray<FGuid> GetGuids(const TArray<FSceneKeyCode>& Codes);
	TArray<FSceneKeyCode> GetCodes(const TArray<FGuid>& Guids);
	FSceneKeyCode GetCode(const FGuid& Guid);
	FGuid GetGuid(const FSceneKeyCode& Code);

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "Name", ForceInlineRow))
	TMap<FString, FSceneGUIDData> SceneCodeGUIs;

	bool Contains(const FSceneKeyCode& Code);
	void Add(const FSceneKeyCode& SceneKeyCode, FGuid Guid);
};