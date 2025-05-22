// Copyright (c) Guillem Serra. All Rights Reserved.

#include "SceneKeyCode.h"

FSceneKeyCode::FSceneKeyCode(FString InKey)
{
	InKey.ToUpperInline();
	const int32 SeparationIndex = InKey.Find(Separation);
	if (SeparationIndex == INDEX_NONE)
	{
		this->Scene = InKey;
		return;
	}

	FString NewScene = InKey.Left(SeparationIndex);
	FString NewCode = InKey.RightChop(SeparationIndex + 1);

	if (NewScene.StartsWith("S"))
	{
		this->Scene = NewScene;
	}
	if (!NewCode.IsEmpty() && !FChar::IsDigit(NewCode[0]))
	{
		this->Code = NewCode;
	}
}

FString FSceneKeyCode::ToString() const
{
	if (Code.IsEmpty())
	{
		return Scene.ToString();
	}

	if (Scene.IsEmpty())
	{
		return Code.ToString();
	}

	return Scene.ToString() + Separation + Code.ToString();
}

FText FSceneKeyCode::ToText() const
{
	return FText::FromString(ToString());
}

bool FSceneKeyCode::Contains(const FString& String) const
{
	return Code.Contains(String);
}

bool FSceneKeyCode::operator==(const FSceneKeyCode& Other) const
{
	return Code == Other.Code && Scene == Other.Scene;
}

bool FSceneKeyCode::operator!=(const FSceneKeyCode& KeyCode) const
{
	return Code != KeyCode.Code || Scene != KeyCode.Scene;
}

FSceneKeyCode::operator FString() const
{
	return ToString();
}

int32 FSceneKeyCode::GetSceneNumber() const
{
	FString NumericPart = Scene.ToString().RightChop(1);
	return FCString::Atoi(*NumericPart);
}

bool FSceneKeyCode::IsEmpty() const
{
	return Scene.IsEmpty() && Code.IsEmpty();
}

bool FSceneKeyCode::IsValid() const
{
	return !Scene.IsEmpty() && !Code.IsEmpty();
}

void FSceneKeyCode::Empty()
{
	Scene.Empty();
	Code.Empty();
}

const FString FSceneKeyCode::Separation = TEXT("-");

bool FSceneKeyCode::IsValid(const FString& InKey)
{
	FSceneKeyCode SceneKeyCode = FSceneKeyCode(InKey);
	return SceneKeyCode.IsValid();
}