#pragma once

#include "CoreMinimal.h"

#include "VgScript/Data/SceneKeyCode.h"

#include "GUIDHandler.generated.h"

struct FCodeGUIDPair;
class UVgScriptGUIDDataAsset;

/**
 * GUIDHandler is responsible for retrieving a GUID based on a static mapping stored in a DataAsset.
 * The mapping is defined via an array of Code to GUID pairs and is cached internally for quick lookup.
 */
UCLASS()
class VGSCRIPTEDITOR_API UGUIDHandler : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	UVgScriptGUIDDataAsset* VgScriptGUIDDataAsset;

	void AddGUID(const FSceneKeyCode& Code);
	void HandleSceneReplacements(TMap<FString, FString> SceneModifications);
	void HandleGUIDReplacements(TMap<FSceneKeyCode, FSceneKeyCode> Map);
	TArray<FString> GetGuids() const;
	void CleanRepeated();
};