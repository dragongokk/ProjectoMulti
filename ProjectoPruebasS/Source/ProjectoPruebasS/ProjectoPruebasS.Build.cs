// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectoPruebasS : ModuleRules
{
	public ProjectoPruebasS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
