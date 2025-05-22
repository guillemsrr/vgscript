// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "VgScript/Data/ActionDataAsset.h"

#include "UObject/Object.h"
#include "Counter.generated.h"

/**
 * 
 */
UCLASS()
class VGSCRIPTEDITOR_API UCounter : public UObject
{
	GENERATED_BODY()

public:
	TArray<FString> Count(TObjectPtr<UActionDataAsset> DataAsset);

private:
	int32 TextCounter;
	int32 TextLineCounter;
	int32 DecisionCounter;
	int32 DecisionLineCounter;
};