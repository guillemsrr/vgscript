// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "DialogueFileManager.generated.h"

/**
 * 
 */
UCLASS()
class VGSCRIPTEDITOR_API UDialogueFileManager : public UObject
{
	GENERATED_BODY()

public:
	void SaveFile(const FString& Path, TObjectPtr<UDataTable> DialogueDataTable);

private:
	const FString ColumnSeparator = ",";

	//FString Text = TEXT("Key,Speaker,Directed at,Audio file,Lines,Time in,Time out,Comment");
	void AddRow(FString& Text, const FString& Key, const FString& Speaker, const FString& DirectedAt,
	            const FString& AudioFile, const FString& Subtitle, const FString& TimeIn, const FString& TimeOut,
	            const FString& Comment);
};