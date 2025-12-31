// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BrickBreaker : ModuleRules
{
	public BrickBreaker(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG",
            "Slate",
            "SlateCore",
            "EnhancedInput"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicDefinitions.Add("_CRT_SECURE_NO_WARNINGS");
        PublicDefinitions.Add("_SCL_SECURE_NO_WARNINGS");
        PublicDefinitions.Add("_CRT_NONSTDC_NO_WARNINGS");
        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
