// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

class FCodeSymbols
{
public:
	inline static const FString CodeStart = "[";
	inline static const FString CodeEnd = "]";
	inline static const FString SceneCode = "S";
	inline static const FString TalkCode = "T";
	inline static const FString DecisionCode = "D";
	inline static const FString ActionCode = "A";
	inline static const FString ConditionCode = "C";
	inline static const FString PersonTalkingCode = "P";
	inline static const FString ConditionTrueCode = "if(";
	inline static const FString ConditionFalseCode = "if!(";
	inline static const FString CodeSeparation = "-";
	inline static const FString ActionValuesStart = "{";
	inline static const FString ActionValuesEnd = "}";
	inline static const FString ActionValueSeparation = ",";
	inline static const FString NextCode = "->(";
	inline static const FString ParallelCode = "||(";
	inline static const FString SameAsCode = "=(";
	inline static const FString ElementStartCode = "(";
	inline static const FString ElementEndCode = ")";
	inline static const FString ActionDefinitionValueCode = ":";
	inline static const FString AlertScriptComment = "!";
	inline static const FString TextCode = "'";
	inline static const FString EndLine = "\n";
	inline static const FString ActorCommentCode = "//(";
	inline static const FString CommentLine = "//";
};