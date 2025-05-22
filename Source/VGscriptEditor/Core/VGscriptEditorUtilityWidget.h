// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "VgScriptEditorUtilityWidget.generated.h"

class UVgScriptGUIDDataAsset;
class UActionDataAsset;
class UDialogueDataAsset;
class UActionSetterBase;

UCLASS(BlueprintType)
class VGSCRIPTEDITOR_API UVgScriptEditorUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	FString ScriptPath = "Scriptwrite";

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UActionDataAsset> ActionsDataAsset;

	UPROPERTY(EditAnywhere)
	TMap<FString, TObjectPtr<UDataTable>> SubtitleDataTables;
	UPROPERTY(EditAnywhere)
	TMap<FString, TObjectPtr<UDataTable>> AudioDataTables;

	UPROPERTY(EditAnywhere, meta = (GetOptions = "GetLanguages"))
	FString CurrentLanguage;
	UPROPERTY(EditDefaultsOnly)
	FString SoundsAssetPath = "Soliloquy/Sounds/Character/Dialogue";
	UPROPERTY(EditDefaultsOnly)
	FString DataTablesPath = "Soliloquy/Monologues/Data/DataTables";
	UPROPERTY(EditDefaultsOnly)
	FString SourceAudioFilesPath = "E:/Documents/Soliloquy_assets/Audio/Dialogue";
	UPROPERTY(EditDefaultsOnly)
	FString SubtitleDataTablePath = "Text";
	UPROPERTY(EditDefaultsOnly)
	FString AudioDaTablePath = "Audio";

	UPROPERTY(EditDefaultsOnly, Instanced)
	TObjectPtr<class ULocalizationArchiveLoader> LocalizationArchiveLoader;

	UFUNCTION(BlueprintCallable)
	void UpdateCodeEnumerations();
	UFUNCTION(BlueprintCallable)
	void CountElements();
	UFUNCTION(BlueprintCallable)
	void GenerateActions();
	void CreateSubtitleDataTable(const FString& Language);
	void CreateAudioDataTable(const FString& Language);
	UFUNCTION(BlueprintCallable)
	void GenerateDataTables();
	UFUNCTION(BlueprintCallable)
	void UpdateSubtitleCSVs();
	UFUNCTION(BlueprintCallable)
	void UpdateSoundNames();
	UFUNCTION(BlueprintCallable)
	void UpdateAudioCSVs();
	UFUNCTION(BlueprintCallable)
	TSet<USoundWave*> GetUnusedAudios();

	//DEMO
	UPROPERTY(EditDefaultsOnly)
	TArray<FString> DemoScenes;
	UPROPERTY(EditAnywhere)
	TMap<FString, TObjectPtr<UDataTable>> DemoSubtitleDataTables;
	UPROPERTY(EditAnywhere)
	TMap<FString, TObjectPtr<UDataTable>> DemoAudioDataTables;
	
	UFUNCTION(BlueprintCallable)
	void ExtractDemoSubtitleCSVs();
	UFUNCTION(BlueprintCallable)
	void ExtractDemoAudioCSVs();
	UFUNCTION(BlueprintCallable)
	void UpdateSubtitleCSVsFromDemo();

private:
	const FString DefaultDialogueLanguage = "ca-ES";

	TArray<FString> GetScript() const;

	UFUNCTION()
	TArray<FString> GetLanguages();

	FString GetSuitableLanguage(const TArray<FString>& AvailableLanguages, const FString& Code);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UVgScriptGUIDDataAsset> VgScriptGUIDDataAsset;
};