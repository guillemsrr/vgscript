// Copyright (c) Noümen Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AudioData.generated.h"

/**
 * 
 */
USTRUCT()
struct VGSCRIPT_API FAudioData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FString Key;

	UPROPERTY(VisibleAnywhere)
	FString Hash;

	//UPROPERTY(EditAnywhere)
	//TSoftObjectPtr<USoundWave> SourceSound;

	UPROPERTY(VisibleAnywhere)
	FString SpokenText;

	UPROPERTY(VisibleAnywhere)
	FName Speaker;

	UPROPERTY(EditAnywhere)
	FString Comment;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundWave> Sound;
};