// Copyright (c) Noümen Interactive. All Rights Reserved.

#include "VgScriptGUIDDataAsset.h"

bool UVgScriptGUIDDataAsset::Contains(const FSceneKeyCode& Code)
{
	FCodeGUIDPair* CodeGUIDPair = GetCodeGUIPair(Code);
	return CodeGUIDPair != nullptr;
}

void UVgScriptGUIDDataAsset::Add(const FSceneKeyCode& SceneKeyCode, FGuid Guid)
{
	if (Contains(SceneKeyCode))
	{
		return;
	}

	if (!SceneCodeGUIs.Contains(SceneKeyCode.Scene))
	{
		FSceneGUIDData SceneGUIDData;
		SceneGUIDData.Name = FName(SceneKeyCode.Scene.ToString());

		SceneCodeGUIs.Add(SceneKeyCode.Scene, SceneGUIDData);
	}

	SceneCodeGUIs[SceneKeyCode.Scene].CodeGUIDPairs.Add(SceneKeyCode.Code, FCodeGUIDPair(SceneKeyCode.Code, Guid));
}

FCodeGUIDPair* UVgScriptGUIDDataAsset::GetCodeGUIPair(const FSceneKeyCode& Code)
{
	for (TTuple<FString, FSceneGUIDData>& Scene : SceneCodeGUIs)
	{
		if (Scene.Key != Code.Scene.ToString())
		{
			continue;
		}

		for (auto& CodeGUIDPair : Scene.Value.CodeGUIDPairs)
		{
			if (CodeGUIDPair.Value.Code.Equals(Code.Code.ToString()))
			{
				return &CodeGUIDPair.Value;
			}
		}

		return nullptr;
	}

	return nullptr;
}

TArray<FGuid> UVgScriptGUIDDataAsset::GetGuids(const TArray<FSceneKeyCode>& Codes)
{
	TArray<FGuid> Guids;
	for (FSceneKeyCode Code : Codes)
	{
		Guids.Add(GetGuid(Code));
	}
	return Guids;
}

TArray<FSceneKeyCode> UVgScriptGUIDDataAsset::GetCodes(const TArray<FGuid>& Guids)
{
	TArray<FSceneKeyCode> Codes;
	for (FGuid Guid : Guids)
	{
		Codes.Add(GetCode(Guid));
	}
	return Codes;
}

FSceneKeyCode UVgScriptGUIDDataAsset::GetCode(const FGuid& Guid)
{
	for (TTuple<FString, FSceneGUIDData> SceneCodeGuI : SceneCodeGUIs)
	{
		for (auto& CodeGUIDPair : SceneCodeGuI.Value.CodeGUIDPairs)
		{
			FCodeGUIDPair CodeGUID = CodeGUIDPair.Value;
			if (CodeGUID.Guid == Guid)
			{
				return FSceneKeyCode(SceneCodeGuI.Key, CodeGUID.Code);
			}
		}
	}

	return FSceneKeyCode();
}

FGuid UVgScriptGUIDDataAsset::GetGuid(const FSceneKeyCode& Code)
{
	for (TTuple<FString, FSceneGUIDData> SceneCodeGuI : SceneCodeGUIs)
	{
		if (SceneCodeGuI.Key != Code.Scene.ToString())
		{
			continue;
		}
		for (auto& CodeGUIDPair : SceneCodeGuI.Value.CodeGUIDPairs)
		{
			FCodeGUIDPair CodeGUID = CodeGUIDPair.Value;
			if (CodeGUID.Code.Equals(Code.Code.ToString()))
			{
				return CodeGUID.Guid;
			}
		}

		return FGuid();
	}

	return FGuid();
}