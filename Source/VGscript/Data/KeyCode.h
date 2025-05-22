// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

#include "KeyCode.generated.h"

USTRUCT()
struct VGSCRIPT_API FKeyCode
{
	GENERATED_BODY()

	FKeyCode() = default;

	FKeyCode(FString InKey)
	{
		InKey.TrimStartAndEndInline();
		this->Code = InKey;
	}

	FString ToString() const
	{
		return Code;
	}

	bool Contains(const FString& String) const
	{
		return Code.Contains(String);
	}

	bool operator==(const FKeyCode& Other) const
	{
		return Code == Other.Code;
	}

	bool operator!=(const FKeyCode& KeyCode) const
	{
		return !Code.Equals(KeyCode.Code);
	}

	FORCEINLINE friend uint32 GetTypeHash(const FKeyCode& KeyCode)
	{
		return GetTypeHash(KeyCode.Code);
	}

	bool operator!() const
	{
		return Code.IsEmpty();
	}

	operator FString() const
	{
		return Code;
	}

	bool IsEmpty() const;
	void Empty();

	int32 GetNumber();
	int32 GetLine();

protected:
	UPROPERTY(EditDefaultsOnly)
	FString Code;
};