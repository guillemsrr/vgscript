// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once
#include "KeyCode.h"

#include "SceneKeyCode.generated.h"

USTRUCT(BlueprintType)
struct VGSCRIPT_API FSceneKeyCode
{
	GENERATED_BODY()

	FSceneKeyCode() = default;

	FSceneKeyCode(const FKeyCode& InSceneKey, const FKeyCode& InKey)
	{
		this->Scene = InSceneKey;
		this->Code = InKey;
	}

	FSceneKeyCode(FString InKey);

	UPROPERTY(EditDefaultsOnly)
	FKeyCode Scene;
	UPROPERTY(EditDefaultsOnly)
	FKeyCode Code;

	FString ToString() const;
	FText ToText() const;

	bool Contains(const FString& String) const;

	bool operator==(const FSceneKeyCode& Other) const;

	bool operator!=(const FSceneKeyCode& KeyCode) const;

	friend uint32 GetTypeHash(const FSceneKeyCode& KeyCode)
	{
		uint32 HashSceneKey = GetTypeHash(KeyCode.Scene);
		uint32 HashKey = GetTypeHash(KeyCode.Code);

		// Combine hashes in a way that keeps their uniqueness
		return HashCombine(HashSceneKey, HashKey);
	}

	bool operator!() const
	{
		return !Code;
	}

	operator FString() const;

	operator FKeyCode() const
	{
		return Code;
	}

	int32 GetSceneNumber() const;

	bool IsEmpty() const;
	bool IsValid() const;

	void Empty();

	static const FString Separation;

	static bool IsValid(const FString& InKey);
};