// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VgScript : ModuleRules
{
	public VgScript(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		bUseUnity = true;

		PublicDependencyModuleNames.AddRange(new[]
			{
				"Core", "Engine"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine"
			}
		);
	}
}