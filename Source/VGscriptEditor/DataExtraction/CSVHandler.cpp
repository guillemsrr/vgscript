// Copyright (c) Noümen Interactive. All Rights Reserved.

#include "CSVHandler.h"

#include "VgScript/Data/Structs/DataTable/AudioData.h"
#include "VgScript/Data/Structs/Talk/TalkLines.h"
#include "VgScript/Data/Structs/Talk/TalkLine.h"

void UCSVHandler::SetTalkDataMap(TMap<FString, FSceneData> Map)
{
	this->SceneData = Map;
}

void UCSVHandler::UpdateSubtitleTable(UDataTable* DataTable)
{
	DataTable->MarkPackageDirty();

	TArray<FString> Hashes;

	TArray<FString> SceneDataKeys;
	SceneData.GenerateKeyArray(SceneDataKeys);
	SceneDataKeys.Sort();
	Algo::Reverse(SceneDataKeys);

	for (const FString& SceneDataKey : SceneDataKeys)
	{
		TArray<FString> TalkDataKeys;
		TMap<FString, FTalkLines>& TalkDataMap = SceneData[SceneDataKey].TalkDataMap;
		TalkDataMap.GenerateKeyArray(TalkDataKeys);
		TalkDataKeys.Sort();
		Algo::Reverse(TalkDataKeys);
		for (const FString& TalkDataKey : TalkDataKeys)
		{
			for (const FTalkLine& Line : TalkDataMap[TalkDataKey].Lines)
			{
				FString Hash = Line.Hash;
				Hashes.Add(Hash);
				FName RowName = FName(Hash);
				const FString ContextString(TEXT("GetLocalizedSubtitle"));
				FSubtitleData* SubtitleData = DataTable->FindRow<FSubtitleData>(RowName, ContextString);
				if (SubtitleData)
				{
					SubtitleData->Key = Line.SceneKey;
					SubtitleData->Source = Line.SourceText;
					SubtitleData->Speaker = TalkDataMap[TalkDataKey].Talker;
					SubtitleData->Comment = Line.Comment;
				}
				else
				{
					FSubtitleData NewSubtitleData = FSubtitleData();
					NewSubtitleData.Key = Line.SceneKey;
					NewSubtitleData.Hash = Line.Hash;
					NewSubtitleData.Comment = Line.Comment;
					NewSubtitleData.Source = Line.SourceText;
					NewSubtitleData.Speaker = TalkDataMap[TalkDataKey].Talker;

					DataTable->AddRow(RowName, NewSubtitleData);
				}
			}
		}
	}

	TArray<FSubtitleData*> SubtitleRows;
	DataTable->GetAllRows<FSubtitleData>(TEXT(""), SubtitleRows);

	for (const TTuple<FString, FSceneData>& Data : SceneData)
	{
		for (const TTuple<FString, FTalkLines>& TalkDataMap : Data.Value.TalkDataMap)
		{
			for (const FTalkLine& Line : TalkDataMap.Value.Lines)
			{
				TArray<FSubtitleData*> SubtitleDatasWithKey = GetSubtitleDatasWithKey(SubtitleRows, Line.SceneKey);
				for (FSubtitleData* DatasWithKey : SubtitleDatasWithKey)
				{
					if (DatasWithKey->Hash != Line.Hash)
					{
						DataTable->RemoveRow(FName(DatasWithKey->Hash));
					}
				}
			}
		}
	}

	for (FSubtitleData* SubtitleRow : SubtitleRows)
	{
		if (Hashes.Contains(SubtitleRow->Hash))
		{
			continue;
		}

		DataTable->RemoveRow(FName(SubtitleRow->Hash));
	}

	SortSubtitleDataTable(DataTable);
}

void UCSVHandler::UpdateAudioTable(UDataTable* AudioDataTable, UDataTable* SubtitleDataTable)
{
	AudioDataTable->MarkPackageDirty();

	TArray<FSubtitleData*> SubtitleRows;
	SubtitleDataTable->GetAllRows<FSubtitleData>(TEXT(""), SubtitleRows);

	TArray<FString> Hashes;
	TArray<FString> SceneDataKeys;
	SceneData.GenerateKeyArray(SceneDataKeys);
	SceneDataKeys.Sort();
	Algo::Reverse(SceneDataKeys);

	for (const FString& SceneDataKey : SceneDataKeys)
	{
		TArray<FString> TalkDataKeys;
		TMap<FString, FTalkLines>& TalkDataMap = SceneData[SceneDataKey].TalkDataMap;
		TalkDataMap.GenerateKeyArray(TalkDataKeys);
		TalkDataKeys.Sort();
		Algo::Reverse(TalkDataKeys);
		for (const FString& TalkDataKey : TalkDataKeys)
		{
			for (const FTalkLine& Line : TalkDataMap[TalkDataKey].Lines)
			{
				FString Hash = Line.Hash;
				if (!Line.DifferentAudioHash.IsEmpty())
				{
					Hash = Line.DifferentAudioHash;
				}

				Hashes.Add(Hash);

				FName RowName = FName(Hash);
				const FString ContextString(TEXT("GetLocalizedSubtitleAudio"));
				FAudioData* AudioData = AudioDataTable->FindRow<FAudioData>(RowName, ContextString);
				if (AudioData)
				{
					AudioData->Key = Line.SceneKey;
					AudioData->Speaker = TalkDataMap[TalkDataKey].Talker;
					AudioData->Comment = Line.Comment;
				}
				else
				{
					FAudioData NewAudioData;
					NewAudioData.Key = Line.SceneKey;
					NewAudioData.Hash = Hash;
					NewAudioData.Comment = Line.Comment;
					NewAudioData.Speaker = TalkDataMap[TalkDataKey].Talker;
					AudioDataTable->AddRow(RowName, NewAudioData);
				}

				AudioData = AudioDataTable->FindRow<FAudioData>(RowName, ContextString);
				if (!ensure(AudioData))
				{
					//why?¿?¿
					continue;
				}

				FSubtitleData* SubtitleData = GetSubtitleDataWithHash(SubtitleRows, Line.Hash);
				if (SubtitleData)
				{
					if (SubtitleData->Translation.IsEmpty() && SubtitleData->AutoTranslation.IsEmpty())
					{
						AudioData->SpokenText = SubtitleData->Source;
					}
					else if (SubtitleData->Translation.IsEmpty())
					{
						AudioData->SpokenText = SubtitleData->AutoTranslation;
					}
					else
					{
						AudioData->SpokenText = SubtitleData->Translation;
					}
				}
			}
		}
	}

	TArray<FAudioData*> AudioRows;
	AudioDataTable->GetAllRows<FAudioData>(TEXT(""), AudioRows);

	for (FSubtitleData* SubtitleRow : SubtitleRows)
	{
		TArray<FAudioData*> AudioDatasWithKeys = GetAudioDatasWithKey(AudioRows, SubtitleRow->Key);
		ensure(AudioDatasWithKeys.Num() == 1);
	}

	for (FAudioData* AudioRow : AudioRows)
	{
		if (AudioRow->SpokenText.IsEmpty())
		{
			AudioDataTable->RemoveRow(FName(AudioRow->Hash));
			continue;
		}

		if (!Hashes.Contains(AudioRow->Hash))
		{
			AudioDataTable->RemoveRow(FName(AudioRow->Hash));
		}
	}

	SortAudioDataTable(AudioDataTable);
}

void UCSVHandler::UpdateSounds(UDataTable* DataTable, const TArray<FAssetData>& AssetDataArray)
{
	DataTable->MarkPackageDirty();

	TArray<FAudioData*> Rows;
	DataTable->GetAllRows<FAudioData>(TEXT(""), Rows);

	for (const FAssetData& AssetData : AssetDataArray)
	{
		FString Hash = AssetData.AssetName.ToString();
		FAudioData* AudioData = GetAudioDataWithHash(Rows, Hash);
		if (AudioData)
		{
			AudioData->Sound = TSoftObjectPtr<USoundWave>(AssetData.ToSoftObjectPath());
		}
	}

	//this can't be here, maybe just create another button
	/*for (FAudioData* Row : Rows)
	{
		if (!Row->Sound.IsNull())
		{
			Row->Sound = TSoftObjectPtr<USoundWave>();
		}
	}*/
}

TSet<USoundWave*> UCSVHandler::GetUsedAudios(UDataTable* AudioDataTable)
{
	TArray<FAudioData*> AudioRows;
	AudioDataTable->GetAllRows<FAudioData>(TEXT(""), AudioRows);

	TSet<USoundWave*> UsedAudios;
	for (FAudioData* Row : AudioRows)
	{
		if (!Row->Sound.IsNull())
		{
			UsedAudios.Add(Row->Sound.Get());
		}
	}

	return UsedAudios;
}

void UCSVHandler::SortSubtitleDataTable(UDataTable* DataTable)
{
	TArray<FSubtitleData*> RawPointers;
	DataTable->GetAllRows<FSubtitleData>(TEXT(""), RawPointers);

	TArray<FSubtitleData> Rows;
	for (FSubtitleData* RowPtr : RawPointers)
	{
		Rows.Add(*RowPtr);
	}

	Rows.Sort([](const FSubtitleData& A, const FSubtitleData& B)
	{
		FSceneKeyCode AKey = FSceneKeyCode(A.Key);
		FSceneKeyCode BKey = FSceneKeyCode(B.Key);

		int32 ASceneNumber = AKey.GetSceneNumber();
		int32 BSceneNumber = BKey.GetSceneNumber();

		int32 ATalkNumber = AKey.Code.GetNumber();
		int32 BTalkNumber = BKey.Code.GetNumber();

		int32 ALineNumber = AKey.Code.GetLine();
		int32 BLineNumber = BKey.Code.GetLine();

		if (ASceneNumber != BSceneNumber)
		{
			return ASceneNumber < BSceneNumber;
		}

		if (ATalkNumber != BTalkNumber)
		{
			return ATalkNumber < BTalkNumber;
		}

		return ALineNumber < BLineNumber;
	});

	DataTable->MarkPackageDirty();

	// Clear the RawPointers and re-add sorted rows
	DataTable->EmptyTable();

	for (const FSubtitleData& Row : Rows)
	{
		DataTable->AddRow(FName(Row.Hash), Row);
	}
}

void UCSVHandler::SortAudioDataTable(UDataTable* DataTable)
{
	TArray<FAudioData*> RawPointers;
	DataTable->GetAllRows<FAudioData>(TEXT(""), RawPointers);

	TArray<FAudioData> Rows;
	for (FAudioData* RowPtr : RawPointers)
	{
		if (RowPtr)
		{
			Rows.Add(*RowPtr);
		}
	}

	Rows.Sort([](const FAudioData& A, const FAudioData& B)
	{
		FSceneKeyCode AKey = FSceneKeyCode(A.Key);
		FSceneKeyCode BKey = FSceneKeyCode(B.Key);

		int32 ASceneNumber = AKey.GetSceneNumber();
		int32 BSceneNumber = BKey.GetSceneNumber();

		int32 ATalkNumber = AKey.Code.GetNumber();
		int32 BTalkNumber = BKey.Code.GetNumber();

		int32 ALineNumber = AKey.Code.GetLine();
		int32 BLineNumber = BKey.Code.GetLine();

		if (ASceneNumber != BSceneNumber)
		{
			return ASceneNumber < BSceneNumber;
		}

		if (ATalkNumber != BTalkNumber)
		{
			return ATalkNumber < BTalkNumber;
		}

		return ALineNumber < BLineNumber;
	});

	DataTable->MarkPackageDirty();

	// Clear the RawPointers and re-add sorted rows
	DataTable->EmptyTable();

	for (const FAudioData& Row : Rows)
	{
		DataTable->AddRow(FName(Row.Hash), Row);
	}
}

FSubtitleData* UCSVHandler::GetSubtitleDataWithHash(TArray<FSubtitleData*> Rows, const FString& Hash)
{
	for (FSubtitleData* Row : Rows)
	{
		if (!Row)
		{
			continue;
		}
		if (Row->Hash.Equals(Hash))
		{
			return Row;
		}
	}

	return nullptr;
}

FAudioData* UCSVHandler::GetAudioDataWithKey(TArray<FAudioData*> Rows, const FString& Key)
{
	for (FAudioData* Row : Rows)
	{
		if (!Row)
		{
			continue;
		}

		if (Row->Key.Equals(Key))
		{
			return Row;
		}
	}

	return nullptr;
}

FAudioData* UCSVHandler::GetAudioDataWithHash(TArray<FAudioData*> Rows, const FString& Hash)
{
	for (FAudioData* Row : Rows)
	{
		if (!Row)
		{
			continue;
		}
		if (Row->Hash.Equals(Hash))
		{
			return Row;
		}
	}

	return nullptr;
}

TArray<FAudioData*> UCSVHandler::GetAudioDatasWithKey(TArray<FAudioData*> Rows, const FString& Key)
{
	TArray<FAudioData*> AudioDatas;
	for (FAudioData* Row : Rows)
	{
		if (!Row)
		{
			continue;
		}

		if (Row->Key.Equals(Key))
		{
			AudioDatas.Add(Row);
		}
	}

	return AudioDatas;
}

TArray<FSubtitleData*> UCSVHandler::GetSubtitleDatasWithKey(TArray<FSubtitleData*> Rows, const FString& Key)
{
	TArray<FSubtitleData*> SubtitleDatas;
	for (FSubtitleData* Row : Rows)
	{
		if (!Row)
		{
			continue;
		}

		if (Row->Key.Equals(Key))
		{
			SubtitleDatas.Add(Row);
		}
	}

	return SubtitleDatas;
}

void UCSVHandler::CopySpecificSubtitleScenes(UDataTable* FromDataTable, UDataTable* ToDemoDataTable)
{
	TArray<FSubtitleData*> FromRows;
	FromDataTable->GetAllRows<FSubtitleData>(TEXT(""), FromRows);
	ToDemoDataTable->MarkPackageDirty();
	ToDemoDataTable->EmptyTable();

	for (FSubtitleData* Row : FromRows)
	{
		FSceneKeyCode SceneKeyCode = FSceneKeyCode(Row->Key);
		if (SpecificScenes.IsEmpty() || SpecificScenes.Contains(SceneKeyCode.Scene))
		{
			ToDemoDataTable->AddRow(FName(Row->Hash), *Row);
		}
	}

	SortSubtitleDataTable(ToDemoDataTable);
}

void UCSVHandler::CopySpecificAudioScenes(UDataTable* FromDataTable, UDataTable* ToDemoDataTable)
{
	TArray<FAudioData*> FromRows;
	FromDataTable->GetAllRows<FAudioData>(TEXT(""), FromRows);
	ToDemoDataTable->MarkPackageDirty();
	ToDemoDataTable->EmptyTable();

	for (FAudioData* Row : FromRows)
	{
		FSceneKeyCode SceneKeyCode = FSceneKeyCode(Row->Key);
		if (SpecificScenes.IsEmpty() || SpecificScenes.Contains(SceneKeyCode.Scene))
		{
			ToDemoDataTable->AddRow(FName(Row->Hash), *Row);
		}
	}

	SortAudioDataTable(ToDemoDataTable);
}