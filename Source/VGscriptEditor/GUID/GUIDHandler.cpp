#include "GUIDHandler.h"

#include "VgScript/Data/GUID/VgScriptGUIDDataAsset.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

void UGUIDHandler::AddGUID(const FSceneKeyCode& Code)
{
	if (VgScriptGUIDDataAsset->Contains(Code))
	{
		return;
	}

	FGuid NewGuid = FGuid::NewGuid();
	VgScriptGUIDDataAsset->Add(Code, NewGuid);
}

void UGUIDHandler::HandleSceneReplacements(TMap<FString, FString> SceneModifications)
{
	if (!VgScriptGUIDDataAsset)
	{
		return;
	}

	if (SceneModifications.IsEmpty())
	{
		return;
	}

	TMap<FString, FSceneGUIDData> OriginalSceneCodeGUIs;
	for (TPair<FString, FSceneGUIDData> SceneEntry : VgScriptGUIDDataAsset->SceneCodeGUIs)
	{
		OriginalSceneCodeGUIs.Add(SceneEntry.Key, SceneEntry.Value);
	}

	for (TTuple<FString, FString> ModifiedCode : SceneModifications)
	{
		FString From = ModifiedCode.Key;
		FString To = ModifiedCode.Value;

		if (OriginalSceneCodeGUIs.Contains(From))
		{
			if (!VgScriptGUIDDataAsset->SceneCodeGUIs.Contains(To))
			{
				VgScriptGUIDDataAsset->SceneCodeGUIs.Add(To);
			}
			VgScriptGUIDDataAsset->SceneCodeGUIs[To] = OriginalSceneCodeGUIs[From];
		}
		else
		{
			VgScriptGUIDDataAsset->SceneCodeGUIs[To].Name = FName(To);
			VgScriptGUIDDataAsset->SceneCodeGUIs[To].CodeGUIDPairs.Empty();
		}
	}
}

void UGUIDHandler::HandleGUIDReplacements(TMap<FSceneKeyCode, FSceneKeyCode> Map)
{
	if (!VgScriptGUIDDataAsset)
	{
		return;
	}

	if (Map.IsEmpty())
	{
		return;
	}

	TMap<FSceneKeyCode, FGuid> OriginalSceneCodeGUIs;
	for (TPair<FString, FSceneGUIDData>& SceneEntry : VgScriptGUIDDataAsset->SceneCodeGUIs)
	{
		for (TPair<FString, FCodeGUIDPair> CodeEntry : SceneEntry.Value.CodeGUIDPairs)
		{
			OriginalSceneCodeGUIs.Add(FSceneKeyCode(SceneEntry.Key, CodeEntry.Key), CodeEntry.Value.Guid);
		}
	}

	for (TTuple<FSceneKeyCode, FSceneKeyCode> ModifiedCode : Map)
	{
		FSceneKeyCode From = ModifiedCode.Key;
		FSceneKeyCode To = ModifiedCode.Value;
		if (To.Code.IsEmpty())
		{
			To.Code = To.Scene;
			To.Scene = From.Scene;
		}

		FCodeGUIDPair* CodeGUIDPairTo = VgScriptGUIDDataAsset->GetCodeGUIPair(To);
		if (!CodeGUIDPairTo)
		{
			AddGUID(To);
			CodeGUIDPairTo = VgScriptGUIDDataAsset->GetCodeGUIPair(To);
		}

		if (!OriginalSceneCodeGUIs.Contains(From))
		{
			FGuid NewGuid = FGuid::NewGuid();
			CodeGUIDPairTo->Guid = NewGuid;
			continue;
		}

		FGuid OriginalGuid = OriginalSceneCodeGUIs[From];
		CodeGUIDPairTo->Guid = OriginalGuid;
	}
}

TArray<FString> UGUIDHandler::GetGuids() const
{
	TArray<FString> Guids;
	for (TPair<FString, FSceneGUIDData>& SceneEntry : VgScriptGUIDDataAsset->SceneCodeGUIs)
	{
		for (TPair<FString, FCodeGUIDPair> CodeEntry : SceneEntry.Value.CodeGUIDPairs)
		{
			Guids.Add(SceneEntry.Key + " - " + CodeEntry.Value.Code + " = " + CodeEntry.Value.Guid.ToString());
		}
	}

	return Guids;
}

void UGUIDHandler::CleanRepeated()
{
	TArray<FGuid> Guids;
	for (TPair<FString, FSceneGUIDData>& SceneEntry : VgScriptGUIDDataAsset->SceneCodeGUIs)
	{
		TMap<FString, FCodeGUIDPair> GuidMap = TMap<FString, FCodeGUIDPair>(SceneEntry.Value.CodeGUIDPairs);
		for (TPair<FString, FCodeGUIDPair> CodeEntry : GuidMap)
		{
			FGuid Guid = CodeEntry.Value.Guid;
			if (Guids.Contains(Guid))
			{
				SceneEntry.Value.CodeGUIDPairs.Remove(CodeEntry.Key);
			}
			else
			{
				Guids.Add(Guid);
			}
		}

		SceneEntry.Value.Name = FName(SceneEntry.Key);
	}
}