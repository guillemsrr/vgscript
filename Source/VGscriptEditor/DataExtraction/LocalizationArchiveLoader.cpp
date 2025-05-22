// Copyright (c) Guillem Serra. All Rights Reserved.

#include "LocalizationArchiveLoader.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

TArray<FSubtitleData> ULocalizationArchiveLoader::Load(const FString& Language)
{
	TArray<FSubtitleData> SubtitleDatas = TArray<FSubtitleData>();

	FString JsonData;
	FString Path = FPaths::ProjectContentDir() + "Localization/Dialogue" / Language / "Dialogue.archive";
	if (!FFileHelper::LoadFileToString(JsonData, *Path))
	{
		return SubtitleDatas;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonData);
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		TArray<TSharedPtr<FJsonValue>> Subnamespaces = JsonObject->GetArrayField(TEXT("Subnamespaces"));
		for (const TSharedPtr<FJsonValue>& SubnamespaceValue : Subnamespaces)
		{
			TSharedPtr<FJsonObject> Subnamespace = SubnamespaceValue->AsObject();
			FString Namespace = Subnamespace->GetStringField(TEXT("Namespace"));
			if (Namespace != "ST_Texts" && Namespace != "ST_Loops")
			{
				continue;
			}
			TArray<TSharedPtr<FJsonValue>> Children = Subnamespace->GetArrayField(TEXT("Children"));
			for (const TSharedPtr<FJsonValue>& ChildValue : Children)
			{
				TSharedPtr<FJsonObject> Child = ChildValue->AsObject();
				FString Translation = Child->GetObjectField(TEXT("Translation"))->GetStringField(TEXT("Text"));
				FString Key = Child->GetStringField(TEXT("Key"));
				//FSubtitleData SubtitleData = FSubtitleData(Key, Translation, "");
				//SubtitleDatas.Add(SubtitleData);
			}
		}
	}

	return SubtitleDatas;
}