#include "GUIDEditorUtilityWidget.h"

#include "VgScript/Data/GUID/VgScriptGUIDDataAsset.h"

void UGUIDEditorUtilityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CodeTextBox)
	{
		CodeTextBox->OnTextChanged.AddDynamic(this, &UGUIDEditorUtilityWidget::OnCodeTextChanged);
	}

	if (GuidTextBox)
	{
		GuidTextBox->OnTextChanged.AddDynamic(this, &UGUIDEditorUtilityWidget::OnGuidTextChanged);
	}
}

void UGUIDEditorUtilityWidget::OnCodeTextChanged(const FText& Text)
{
	FGuid Guid = VgScriptGUIDDataAsset->GetGuid(Text.ToString());
	GuidTextBox->SetText(FText::FromString(Guid.ToString()));
}

void UGUIDEditorUtilityWidget::OnGuidTextChanged(const FText& Text)
{
	FString Code = VgScriptGUIDDataAsset->GetCode(FGuid(Text.ToString()));
	CodeTextBox->SetText(FText::FromString(Code));
}

void UGUIDEditorUtilityWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!VgScriptGUIDDataAsset)
	{
		VgScriptGUIDDataAsset = Cast<UVgScriptGUIDDataAsset>(StaticLoadObject(
			UVgScriptGUIDDataAsset::StaticClass(),
			this,
			TEXT("/Game/Soliloquy/Narrative/Data/DA_VgScriptGUID.DA_VgScriptGUID")));
	}

	if (!VgScriptGUIDDataAsset)
	{
		return;
	}

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(
		UGUIDEditorUtilityWidget,
		GuidTextBox))
	{
		FString Code = VgScriptGUIDDataAsset->GetCode(FGuid(GuidTextBox->GetText().ToString()));
		CodeTextBox->SetText(FText::FromString(Code));
	}
	else if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(
		UGUIDEditorUtilityWidget,
		CodeTextBox))
	{
		FGuid Guid = VgScriptGUIDDataAsset->GetGuid(CodeTextBox->GetText().ToString());
		GuidTextBox->SetText(FText::FromString(Guid.ToString()));
	}
}