// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SubtitleData.generated.h"

USTRUCT()
struct VGSCRIPT_API FSubtitleData : public FTableRowBase
{
	GENERATED_BODY()

	FSubtitleData() = default;

	/*FSubtitleData()
	{
		Key = "";
		Hash = "";
		Source = "";
		AutoTranslation = "";
		Translation = "";
		Speaker = "";
		Comment = "";
	}*/

	UPROPERTY(VisibleAnywhere)
	FString Key;

	UPROPERTY(VisibleAnywhere)
	FString Hash;

	UPROPERTY(VisibleAnywhere)
	FString Source;

	UPROPERTY(EditAnywhere)
	FString AutoTranslation;

	UPROPERTY(EditAnywhere)
	FString Translation;

	UPROPERTY(VisibleAnywhere)
	FName Speaker;

	UPROPERTY(EditAnywhere)
	FString Comment;
};