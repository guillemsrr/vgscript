// Copyright (c) Guillem Serra. All Rights Reserved.

#include "KeyCode.h"

bool FKeyCode::IsEmpty() const
{
	return Code.IsEmpty();
}

void FKeyCode::Empty()
{
	Code.Empty();
}

int32 FKeyCode::GetNumber()
{
	int32 StartIndex = -1;
	int32 EndIndex = -1;

	// Find the index of the first numeric character and the hyphen
	for (int32 i = 0; i < Code.Len(); ++i)
	{
		if (FChar::IsDigit(Code[i]))
		{
			if (StartIndex == -1)
			{
				StartIndex = i;
			}
		}
		else if (Code[i] == '-')
		{
			EndIndex = i;
			break;
		}
	}

	// Ensure we have valid start and end indices
	if (StartIndex != -1 && EndIndex != -1 && EndIndex > StartIndex)
	{
		// Extract the numeric part before the hyphen
		FString NumericPart = Code.Mid(StartIndex, EndIndex - StartIndex);
		return FCString::Atoi(*NumericPart);
	}
	else if (StartIndex != -1)
	{
		// No hyphen found, but we have a valid start index, extract up to the end of the string
		FString NumericPart = Code.Mid(StartIndex);
		return FCString::Atoi(*NumericPart);
	}

	// Return 0 if no valid numeric part is found
	return 0;
}

int32 FKeyCode::GetLine()
{
	int32 HyphenIndex = Code.Find(TEXT("-"));
	FString NumericPart = Code.Mid(HyphenIndex + 1);
	return FCString::Atoi(*NumericPart);
}