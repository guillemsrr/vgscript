#include "TalkLine.h"

void FTalkLine::Copy(const FTalkLine* TalkLine)
{
	SourceText = TalkLine->SourceText;
	Hash = TalkLine->Hash;
	DifferentAudioHash = TalkLine->DifferentAudioHash;
	Text = TalkLine->Text;
	Comment = TalkLine->Comment;

	if (Condition.IsEmpty())
	{
		Condition = TalkLine->Condition;
	}

	if (Modifiers.IsEmpty())
	{
		Modifiers = TalkLine->Modifiers;
	}

	if (NextKeys.IsEmpty())
	{
		NextKeys = TalkLine->NextKeys;
	}

	if (NextInlineActionDatas.IsEmpty())
	{
		NextInlineActionDatas = TalkLine->NextInlineActionDatas;
	}

	if (ParallelKeys.IsEmpty())
	{
		ParallelKeys = TalkLine->ParallelKeys;
	}

	if (ParallelInlineActionDatas.IsEmpty())
	{
		ParallelInlineActionDatas = TalkLine->ParallelInlineActionDatas;
	}
}

FString FTalkLine::GetAudioHash()
{
	if (!DifferentAudioHash.IsEmpty())
	{
		return DifferentAudioHash;
	}

	return Hash;
}