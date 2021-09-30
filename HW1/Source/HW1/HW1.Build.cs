// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HW1 : ModuleRules
{
	public HW1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
