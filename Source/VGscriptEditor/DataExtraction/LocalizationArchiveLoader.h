// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "VgScript/Data/Structs/DataTable/SubtitleData.h"

#include "UObject/Object.h"
#include "LocalizationArchiveLoader.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew)
class VGSCRIPTEDITOR_API ULocalizationArchiveLoader : public UObject
{
	GENERATED_BODY()

public:
	TArray<FSubtitleData> Load(const FString& Language);
};