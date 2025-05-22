// Copyright (c) Noümen Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VgScript/Data/SceneData.h"
#include "VgScript/Data/Structs/DataTable/AudioData.h"
#include "VgScript/Data/Structs/DataTable/SubtitleData.h"

#include "UObject/Object.h"
#include "CSVHandler.generated.h"

/**
 * 
 */
UCLASS()
class VGSCRIPTEDITOR_API UCSVHandler : public UObject
{
	GENERATED_BODY()

public:
	void SetTalkDataMap(TMap<FString, FSceneData> Map);
	void UpdateSubtitleTable(UDataTable* DataTable);
	void UpdateAudioTable(UDataTable* AudioDataTable, UDataTable* SubtitleDataTable);
	void UpdateSounds(UDataTable* DataTable, const TArray<FAssetData>& AssetDataArray);

	TSet<USoundWave*> GetUsedAudios(UDataTable* AudioDataTable);

	FAudioData* GetAudioDataWithKey(TArray<FAudioData*> Rows, const FString& Key);
	void CopySpecificSubtitleScenes(UDataTable* FromDataTable, UDataTable* ToDemoDataTable);
	void CopySpecificAudioScenes(UDataTable* FromDataTable, UDataTable* ToDemoDataTable);

	UPROPERTY(Transient)
	TArray<FString> SpecificScenes;

private:
	UPROPERTY(Transient)
	TMap<FString, FSceneData> SceneData;

	FSubtitleData* GetSubtitleDataWithHash(TArray<FSubtitleData*> Rows, const FString& Hash);
	FAudioData* GetAudioDataWithHash(TArray<FAudioData*> Rows, const FString& Hash);
	TArray<FAudioData*> GetAudioDatasWithKey(TArray<FAudioData*> Rows, const FString& Key);
	TArray<FSubtitleData*> GetSubtitleDatasWithKey(TArray<FSubtitleData*> Rows, const FString& Key);

	void SortSubtitleDataTable(UDataTable* DataTable);
	void SortAudioDataTable(UDataTable* DataTable);
};