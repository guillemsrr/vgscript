// Copyright (c) Noümen Interactive. All Rights Reserved.

#pragma once

#include "CodeGUIDPair.generated.h"

/**
 * A simple struct representing one mapping from a human-readable code to a GUID.
 */
USTRUCT(BlueprintType)
struct VGSCRIPT_API FCodeGUIDPair
{
	GENERATED_BODY()

	FCodeGUIDPair() = default;

	FCodeGUIDPair(FString InCode, FGuid InGuid);

	UPROPERTY(EditAnywhere)
	FString Code;

	UPROPERTY(EditAnywhere)
	FGuid Guid;
};