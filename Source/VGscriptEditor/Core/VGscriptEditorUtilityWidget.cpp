// Copyright (c) Guillem Serra. All Rights Reserved.

#include "VGscriptEditorUtilityWidget.h"
#include "AssetToolsModule.h"
#include "CodeSymbols.h"
#include "EditorAssetLibrary.h"
#include "EngineUtils.h"
#include "VgScript/Data/ActionDataAsset.h"
#include "VgScript/Data/GUID/VgScriptGUIDDataAsset.h"

#include "VgScriptEditor/DataExtraction/CSVHandler.h"
#include "VgScriptEditor/DataExtraction/DataExtractor.h"
#include "VgScriptEditor/DataExtraction/LocalizationArchiveLoader.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "VgScriptEditor/Enumerator/CodeEnumerator.h"
#include "VgScriptEditor/Enumerator/Counter.h"
#include "VgScriptEditor/GUID/GUIDHandler.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Misc/FileHelper.h"
#include "Sound/SoundWave.h"

#include "UObject/SavePackage.h"

void UVgScriptEditorUtilityWidget::UpdateCodeEnumerations()
{
	if (!VgScriptGUIDDataAsset)
	{
		return;
	}

	FString CurrentTime = FDateTime::Now().ToString();
	FString Path = UKismetSystemLibrary::GetProjectDirectory() / ScriptPath / CurrentTime / TEXT("Backup.txt");
	FString OriginalPath = UKismetSystemLibrary::GetProjectDirectory() / ScriptPath / TEXT("Soliloqui_Guió.txt");
	TArray<FString> FullScriptWithParagraphs;
	FFileHelper::LoadFileToStringArray(FullScriptWithParagraphs, *OriginalPath);
	FString JoinedScript = FString::Join(FullScriptWithParagraphs, *FCodeSymbols::EndLine);
	FFileHelper::SaveStringToFile(JoinedScript, *Path);

	UCodeEnumerator* CodeEnumerator = NewObject<UCodeEnumerator>(this);
	UGUIDHandler* GUIDHandler = NewObject<UGUIDHandler>(this);
	GUIDHandler->VgScriptGUIDDataAsset = VgScriptGUIDDataAsset;
	VgScriptGUIDDataAsset->MarkPackageDirty();

	CodeEnumerator->GUIDHandler = GUIDHandler;
	TArray<FString> Script = GetScript();
	CodeEnumerator->UpdateCodeEnumerations(Script);
	JoinedScript = FString::Join(Script, *FCodeSymbols::EndLine);
	Path = UKismetSystemLibrary::GetProjectDirectory() / ScriptPath / CurrentTime / TEXT("UpdatedScript.txt");
	FFileHelper::SaveStringToFile(JoinedScript, *Path);
	FFileHelper::SaveStringToFile(JoinedScript, *OriginalPath); //directly override the original path

	Path = UKismetSystemLibrary::GetProjectDirectory() / ScriptPath / CurrentTime / TEXT("Changes.txt");
	TArray<FString> Changes = CodeEnumerator->GetChanges();
	Changes.Insert(CurrentTime, 0);
	FString JoinedChanges = FString::Join(Changes, *FCodeSymbols::EndLine);
	FFileHelper::SaveStringToFile(JoinedChanges, *Path);

	Path = UKismetSystemLibrary::GetProjectDirectory() / ScriptPath / CurrentTime / TEXT("GUID_Update.txt");
	TArray<FString> GuidChanges = GUIDHandler->GetGuids();
	Changes.Insert(CurrentTime, 0);
	FString JoinedGuidChanges = FString::Join(GuidChanges, *FCodeSymbols::EndLine);
	FFileHelper::SaveStringToFile(JoinedGuidChanges, *Path);
}

void UVgScriptEditorUtilityWidget::CountElements()
{
	UCounter* Counter = NewObject<UCounter>(this);
	TArray<FString> Script = Counter->Count(ActionsDataAsset);

	FString Path = UKismetSystemLibrary::GetProjectDirectory() / ScriptPath / TEXT("Count.txt");
	FString JoinedScript = FString::Join(Script, *FCodeSymbols::EndLine);
	FFileHelper::SaveStringToFile(JoinedScript, *Path);
}

TArray<FString> UVgScriptEditorUtilityWidget::GetScript() const
{
	TArray<FString> Script;
	FString Path = UKismetSystemLibrary::GetProjectDirectory() / ScriptPath / TEXT("Soliloqui_Guió.txt");

	TArray<FString> FullScriptWithParagraphs;
	if (!FFileHelper::LoadFileToStringArray(FullScriptWithParagraphs, *Path))
	{
		GLog->Log("Error");
		return Script;
	}

	for (FString& Line : FullScriptWithParagraphs)
	{
		if (!Line.IsEmpty())
		{
			Script.Add(Line);
		}
	}

	return Script;
}

void UVgScriptEditorUtilityWidget::GenerateActions()
{
	TArray<FString> Script = GetScript();

	ActionsDataAsset->SceneData.Empty();
	ActionsDataAsset->MarkPackageDirty();

	UDataExtractor* DataExtractor = NewObject<UDataExtractor>(this);
	DataExtractor->SetActionsData(ActionsDataAsset);

	for (FString Line : Script)
	{
		DataExtractor->ExtractLineData(Line);
	}

	DataExtractor->UpdateUnfinishedActions();
}

void UVgScriptEditorUtilityWidget::GenerateDataTables()
{
	TArray<FString> Languages = GetLanguages();
	for (const FString& Language : Languages)
	{
		CreateSubtitleDataTable(Language);
		CreateAudioDataTable(Language);
	}

	MarkPackageDirty();
}

void UVgScriptEditorUtilityWidget::CreateSubtitleDataTable(const FString& Language)
{
	FString DataTableAssetPath = "/Game" / DataTablesPath / SubtitleDataTablePath / Language;
	UDataTable* DataTable = LoadObject<UDataTable>(this, *DataTableAssetPath);
	if (!DataTable)
	{
		// Create a new DataTable
		DataTable = NewObject<UDataTable>(this,
		                                  UDataTable::StaticClass(),
		                                  FName(DataTableAssetPath),
		                                  RF_Public | RF_Standalone);
		DataTable->RowStruct = FSubtitleData::StaticStruct();

		// Save the DataTable as an asset
		FAssetRegistryModule::AssetCreated(DataTable);
		FString PackageName = DataTableAssetPath;
		UPackage* Package = CreatePackage(*PackageName);
		DataTable->Rename(*FPaths::GetBaseFilename(PackageName), Package);
		Package->MarkPackageDirty();

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error = GError;
		SaveArgs.bWarnOfLongFilename = false;
		SaveArgs.SaveFlags = SAVE_NoError;

		if (UPackage::SavePackage(Package,
		                          DataTable,
		                          *(PackageName + FPackageName::GetAssetPackageExtension()),
		                          SaveArgs))
		{
			UE_LOG(LogTemp, Log, TEXT("Created and saved new DataTable for language: %s"), *Language);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save new DataTable for language: %s"), *Language);
			return;
		}
	}

	// not working
	//SubtitleDataTables.Add(Language, DataTable);
}

void UVgScriptEditorUtilityWidget::CreateAudioDataTable(const FString& Language)
{
	FString DataTableAssetPath = "/Game" / DataTablesPath / AudioDaTablePath / Language;
	UDataTable* DataTable = LoadObject<UDataTable>(this, *DataTableAssetPath);
	if (!DataTable)
	{
		// Create a new DataTable
		DataTable = NewObject<UDataTable>(this,
		                                  UDataTable::StaticClass(),
		                                  FName(DataTableAssetPath),
		                                  RF_Public | RF_Standalone);
		DataTable->RowStruct = FAudioData::StaticStruct();

		// Save the DataTable as an asset
		FAssetRegistryModule::AssetCreated(DataTable);
		FString PackageName = DataTableAssetPath;
		UPackage* Package = CreatePackage(*PackageName);
		DataTable->Rename(*FPaths::GetBaseFilename(PackageName), Package);
		Package->MarkPackageDirty();

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error = GError;
		SaveArgs.bWarnOfLongFilename = false;
		SaveArgs.SaveFlags = SAVE_NoError;

		if (UPackage::SavePackage(Package,
		                          DataTable,
		                          *(PackageName + FPackageName::GetAssetPackageExtension()),
		                          SaveArgs))
		{
			UE_LOG(LogTemp, Log, TEXT("Created and saved new DataTable for language: %s"), *Language);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save new DataTable for language: %s"), *Language);
			return;
		}
	}

	//AudioDataTables.Add(Language, DataTable);
}

void UVgScriptEditorUtilityWidget::UpdateSubtitleCSVs()
{
	TArray<FString> Keys;
	SubtitleDataTables.GetKeys(Keys);

	UCSVHandler* CSVHandler = NewObject<UCSVHandler>(this);
	CSVHandler->SetTalkDataMap(ActionsDataAsset->SceneData);
	for (const FString& Language : GetLanguages())
	{
		FString Code = GetSuitableLanguage(Keys, Language);
		if (!SubtitleDataTables.Contains(Code))
		{
			continue;
		}
		UDataTable* DataTable = SubtitleDataTables[Code];
		CSVHandler->UpdateSubtitleTable(DataTable);
	}
}

TArray<FString> UVgScriptEditorUtilityWidget::GetLanguages()
{
	return UKismetInternationalizationLibrary::GetLocalizedCultures();
}

FString UVgScriptEditorUtilityWidget::GetSuitableLanguage(const TArray<FString>& AvailableLanguages,
                                                          const FString& Code)
{
	return UKismetInternationalizationLibrary::GetSuitableCulture(AvailableLanguages, Code, "none");
}

void UVgScriptEditorUtilityWidget::UpdateSoundNames()
{
	TArray<FString> Keys;
	AudioDataTables.GetKeys(Keys);

	for (const FString& Language : GetLanguages())
	{
		FString Code = GetSuitableLanguage(Keys, Language);
		UDataTable* AudioDataTable = AudioDataTables[Code];
		if (!AudioDataTable)
		{
			continue;
		}

		TArray<FAudioData*> AudioDatas;
		AudioDataTable->GetAllRows<FAudioData>(TEXT(""), AudioDatas);

		FString Path = FPaths::Combine(SourceAudioFilesPath, Code);
		if (!IFileManager::Get().DirectoryExists(*Path))
		{
			UE_LOG(LogTemp, Warning, TEXT("Directory does not exist: %s"), *Path);
			continue;
		}

		UCSVHandler* CSVHandler = NewObject<UCSVHandler>(this);

		TArray<FString> FoundFiles;
		IFileManager::Get().FindFiles(FoundFiles, *Path, TEXT("*.wav")); // Filter for .wav files
		if (FoundFiles.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("No audio files found in directory: %s"), *Path);
			continue;
		}

		TArray<FString> FilesToImport;
		for (const FString& FileName : FoundFiles)
		{
			FString Key = FPaths::GetBaseFilename(FileName);
			FAudioData* AudioData = CSVHandler->GetAudioDataWithKey(AudioDatas, Key);
			if (!ensure(AudioData))
			{
				continue;
			}

			FString Hash = AudioData->Hash;
			Hash = FPaths::MakeValidFileName(Hash);
			FString FilePath = FPaths::Combine(Path, FileName);
			FString NewFilePath = FPaths::Combine(Path + " - hash", Hash + TEXT(".wav"));

			if (bool bFileMoved = IFileManager::Get().Copy(*NewFilePath, *FilePath) == COPY_OK)
			{
				FilesToImport.Add(NewFilePath);
				UE_LOG(LogTemp, Log, TEXT("Renamed file: %s -> %s"), *FilePath, *NewFilePath);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to rename file: %s"), *FilePath);
			}
		}

		TArray<FString> FilesToImportCopy = FilesToImport;

		FString ContentDir = FPaths::ProjectContentDir();
		FString TargetPath = FPaths::Combine(ContentDir, SoundsAssetPath, Code);
		FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
		for (FString ToImport : FilesToImportCopy)
		{
			// Find asset data using the file path
			FAssetData Asset = UEditorAssetLibrary::FindAssetData(ToImport);

			if (Asset.IsValid())
			{
				// Get the existing file's path (from asset data)
				FString ExistingAssetPath = Asset.GetSoftObjectPath().ToString();
				FString ExistingFilePath = FPaths::ConvertRelativePathToFull(ExistingAssetPath);

				// Compare file sizes of the existing asset and the file to import
				int64 ExistingFileSize = IFileManager::Get().FileSize(*ExistingFilePath);
				int64 NewFileSize = IFileManager::Get().FileSize(*ToImport);

				if (ExistingFileSize == NewFileSize)
				{
					// If the file sizes are the same, remove it from the import list
					FilesToImport.Remove(ToImport);
					UE_LOG(LogTemp,
					       Log,
					       TEXT("Skipping import of file (already exists with same size): %s"),
					       *ToImport);
				}
			}
		}
		TArray<UObject*> ImportedAssets = AssetToolsModule.Get().ImportAssets(FilesToImport, TargetPath);

		if (ImportedAssets.Num() == 0 || !ImportedAssets[0])
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to import file: %s"), *TargetPath);
		}
	}
}

void UVgScriptEditorUtilityWidget::UpdateAudioCSVs()
{
	TArray<FString> Keys;
	AudioDataTables.GetKeys(Keys);

	UCSVHandler* CSVHandler = NewObject<UCSVHandler>(this);
	CSVHandler->SetTalkDataMap(ActionsDataAsset->SceneData);
	for (const FString& Language : GetLanguages())
	{
		FString Code = GetSuitableLanguage(Keys, Language);
		if (!AudioDataTables.Contains(Code))
		{
			continue;
		}
		UDataTable* DataTable = AudioDataTables[Code];
		UDataTable* SubtitleDataTable = SubtitleDataTables[Code];
		CSVHandler->UpdateAudioTable(DataTable, SubtitleDataTable);

		TArray<FAssetData> SoundAssets;
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
			"AssetRegistry");
		FString ContentDir = FPaths::ProjectContentDir();
		FString TargetPath = FPaths::Combine(TEXT("/Game"), SoundsAssetPath, Code);
		AssetRegistryModule.Get().GetAssetsByPath(*TargetPath, SoundAssets, true);
		CSVHandler->UpdateSounds(DataTable, SoundAssets);
	}
}

TSet<USoundWave*> UVgScriptEditorUtilityWidget::GetUnusedAudios()
{
	TSet<USoundWave*> UnusedSounds;
	TArray<FString> Keys;
	AudioDataTables.GetKeys(Keys);

	UCSVHandler* CSVHandler = NewObject<UCSVHandler>(this);
	CSVHandler->SetTalkDataMap(ActionsDataAsset->SceneData);
	for (const FString& Language : GetLanguages())
	{
		FString Code = GetSuitableLanguage(Keys, Language);
		if (!AudioDataTables.Contains(Code))
		{
			continue;
		}
		UDataTable* DataTable = AudioDataTables[Code];

		TArray<FAssetData> SoundAssets;
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
			"AssetRegistry");
		FString ContentDir = FPaths::ProjectContentDir();
		FString TargetPath = FPaths::Combine(TEXT("/Game"), SoundsAssetPath, Code);
		AssetRegistryModule.Get().GetAssetsByPath(*TargetPath, SoundAssets, true);
		TSet<USoundWave*> Audios = CSVHandler->GetUsedAudios(DataTable);
		for (const FAssetData& AssetData : SoundAssets)
		{
			USoundWave* SoundWave = Cast<USoundWave>(AssetData.GetAsset());
			if (SoundWave && !Audios.Contains(SoundWave))
			{
				UnusedSounds.Add(SoundWave);
			}
		}
	}

	return UnusedSounds;
}

void UVgScriptEditorUtilityWidget::ExtractDemoSubtitleCSVs()
{
	TArray<FString> Keys;
	DemoSubtitleDataTables.GetKeys(Keys);

	UCSVHandler* CSVHandler = NewObject<UCSVHandler>(this);
	CSVHandler->SpecificScenes = DemoScenes;
	CSVHandler->SetTalkDataMap(ActionsDataAsset->SceneData);
	for (const FString& Language : GetLanguages())
	{
		FString Code = GetSuitableLanguage(Keys, Language);
		if (!DemoSubtitleDataTables.Contains(Code))
		{
			continue;
		}
		UDataTable* DataTable = SubtitleDataTables[Code];
		UDataTable* DemoDataTable = DemoSubtitleDataTables[Code];
		CSVHandler->CopySpecificSubtitleScenes(DataTable, DemoDataTable);
	}
}

void UVgScriptEditorUtilityWidget::ExtractDemoAudioCSVs()
{
	TArray<FString> Keys;
	DemoAudioDataTables.GetKeys(Keys);

	UCSVHandler* CSVHandler = NewObject<UCSVHandler>(this);
	CSVHandler->SpecificScenes = DemoScenes;
	CSVHandler->SetTalkDataMap(ActionsDataAsset->SceneData);
	for (const FString& Language : GetLanguages())
	{
		FString Code = GetSuitableLanguage(Keys, Language);
		if (!DemoAudioDataTables.Contains(Code))
		{
			continue;
		}
		CSVHandler->CopySpecificAudioScenes(AudioDataTables[Code], DemoAudioDataTables[Code]);
	}
}

void UVgScriptEditorUtilityWidget::UpdateSubtitleCSVsFromDemo()
{
	TArray<FString> Keys;
	DemoSubtitleDataTables.GetKeys(Keys);

	UCSVHandler* CSVHandler = NewObject<UCSVHandler>(this);
	CSVHandler->SetTalkDataMap(ActionsDataAsset->SceneData);
	for (const FString& Language : GetLanguages())
	{
		FString Code = GetSuitableLanguage(Keys, Language);
		if (!DemoSubtitleDataTables.Contains(Code))
		{
			continue;
		}
		UDataTable* DemoDataTable = DemoSubtitleDataTables[Code];
		UDataTable* DataTable = SubtitleDataTables[Code];
		CSVHandler->CopySpecificSubtitleScenes(DemoDataTable, DataTable);
	}
}