// Copyright (c) Guillem Serra. All Rights Reserved.

#include "ActionValue.h"

FString FActionValue::ToString() const
{
	if (CodeName.IsEmpty())
	{
		return Value;
	}

	if (Value.IsEmpty())
	{
		return CodeName;
	}

	return CodeName + ": " + Value;
}

bool FActionValue::IsEmpty() const
{
	return CodeName.IsEmpty() && Value.IsEmpty();
}