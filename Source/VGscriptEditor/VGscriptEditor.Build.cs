using UnrealBuildTool;

public class VgScriptEditor : ModuleRules
{
	public VgScriptEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		bUseUnity = true;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core", "Engine", "VgScript"
			}
		);

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Projects",
			"InputCore",
			"EditorFramework",
			"UnrealEd",
			"ToolMenus",
			"CoreUObject",
			"Slate",
			"SlateCore",
			"Blutility",
			"UMG",
			"ApplicationCore",
			"PropertyPath",
			"Json",
			"JsonUtilities",
			"HTTP",
			"EditorScriptingUtilities"
		});
	}
}