// Copyright (c) Guillem Serra. All Rights Reserved.

#include "DialogueFileManager.h"
#include "LocTextHelper.h"
#include "Internationalization/StringTable.h"

#include "Kismet/KismetInternationalizationLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetTextLibrary.h"

#include "Misc/FileHelper.h"

#include "Serialization/Csv/CsvParser.h"

void UDialogueFileManager::SaveFile(const FString& Path, TObjectPtr<UDataTable> DialogueDataTable)
{
	FString FileName = Path / "Test.csv";
	FString Text = TEXT("Key,Speaker,Directed at,Audio file,Lines,Time in,Time out,Comment");

	FString BaseReportCSV;
	if (FFileHelper::LoadFileToString(BaseReportCSV, *FileName))
	{
		//TODO: update existing CSV
		FCsvParser CsvParser(BaseReportCSV);
	}
	else
	{
		//Create new CSV
	}

	for (FName RowName : DialogueDataTable->GetRowNames())
	{
		//FSubtitledDialogue* SubtitledDialogue = DialogueDataTable->FindRow<FSubtitledDialogue>(RowName, "");
		/*FString Speaker = SubtitledDialogue->Speaker->GetName(); //TODO: should be translated
		FString DirectedAt = SubtitledDialogue->DirectedAt->GetName(); //TODO: should be translated
		FString AudioFile = "Audio_" + RowName.ToString();
		FString Subtitle = SubtitledDialogue->Subtitles[0].ToString(); //TODO: Get Specific localized text
		FString Comment = SubtitledDialogue->Comment.ToString(); //TODO: Get Specific localized Comment

		AddRow(Text, RowName.ToString(), Speaker, DirectedAt, AudioFile, Subtitle, "", "", Comment);

		for (int i = 1; i < SubtitledDialogue->Subtitles.Num(); ++i)
		{
			Subtitle = SubtitledDialogue->Subtitles[i].ToString();
			AddRow(Text, "", "", "", "", Subtitle, "", "", "");
		}*/
	}

	FFileHelper::SaveStringToFile(Text, *FileName);
}

void UDialogueFileManager::AddRow(FString& Text, const FString& Key, const FString& Speaker, const FString& DirectedAt,
                                  const FString& AudioFile, const FString& Subtitle, const FString& TimeIn,
                                  const FString& TimeOut, const FString& Comment)
{
	Text += Key;
	Text += ColumnSeparator;
	Text += Speaker;
	Text += ColumnSeparator;
	Text += DirectedAt;
	Text += ColumnSeparator;
	Text += AudioFile;
	Text += ColumnSeparator;
	Text += Subtitle;
	Text += ColumnSeparator;
	Text += TimeIn;
	Text += ColumnSeparator;
	Text += TimeOut;
	Text += ColumnSeparator;
	Text += Comment;
	Text += LINE_TERMINATOR;
}