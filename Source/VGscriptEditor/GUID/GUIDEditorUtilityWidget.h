// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetComponents.h"
#include "GUIDEditorUtilityWidget.generated.h"

class UVgScriptGUIDDataAsset;

UCLASS(BlueprintType)
class VGSCRIPTEDITOR_API UGUIDEditorUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UEditorUtilityEditableTextBox* CodeTextBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UEditorUtilityEditableTextBox* GuidTextBox;

	UPROPERTY(EditDefaultsOnly)
	UVgScriptGUIDDataAsset* VgScriptGUIDDataAsset;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCodeTextChanged(const FText& Text);

	UFUNCTION()
	void OnGuidTextChanged(const FText& Text);
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};