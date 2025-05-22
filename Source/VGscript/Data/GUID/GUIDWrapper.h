// Copyright (c) Noümen Interactive. All Rights Reserved.

#pragma once

#include "GUIDWrapper.generated.h"

/**
 * A simple struct representing one mapping from a human-readable code to a GUID.
 */
UCLASS(EditInlineNew, DefaultToInstanced, BlueprintType)
class VGSCRIPT_API UGUIDWrapper: public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FString Code;

	UPROPERTY(EditAnywhere)
	FGuid Guid;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
};