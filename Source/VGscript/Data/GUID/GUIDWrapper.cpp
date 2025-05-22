#include "GUIDWrapper.h"
#include "VgScriptGUIDDataAsset.h"

#if WITH_EDITOR

void UGUIDWrapper::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	UVgScriptGUIDDataAsset* VgScriptGUIDDataAsset = Cast<UVgScriptGUIDDataAsset>(StaticLoadObject(
		UVgScriptGUIDDataAsset::StaticClass(),
		this,
		TEXT("/Game/Soliloquy/Narrative/Data/DA_VgScriptGUID.DA_VgScriptGUID")));

	if (!VgScriptGUIDDataAsset)
	{
		return;
	}

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(
		UGUIDWrapper,
		Guid))
	{
		Code = VgScriptGUIDDataAsset->GetCode(Guid);
	}
	else if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(
		UGUIDWrapper,
		Code))
	{
		Guid = VgScriptGUIDDataAsset->GetGuid(Code);
	}
}

#endif