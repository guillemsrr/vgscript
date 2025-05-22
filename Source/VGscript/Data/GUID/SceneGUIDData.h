// Copyright (c) Noümen Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CodeGUIDPair.h"

#include "UObject/Object.h"
#include "SceneGUIDData.generated.h"

/**
 * 
 */
USTRUCT()
struct VGSCRIPT_API FSceneGUIDData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere, meta = (ForceInlineRow))
	TMap<FString, FCodeGUIDPair> CodeGUIDPairs;
};