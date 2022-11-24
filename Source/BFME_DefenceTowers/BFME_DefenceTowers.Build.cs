// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BFME_DefenceTowers : ModuleRules
{
	public BFME_DefenceTowers(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
    }
}
