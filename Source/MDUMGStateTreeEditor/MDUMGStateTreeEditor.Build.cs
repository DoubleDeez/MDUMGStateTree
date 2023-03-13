using UnrealBuildTool;

public class MDUMGStateTreeEditor : ModuleRules
{
    public MDUMGStateTreeEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "UMGEditor"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Kismet",
                "MDUMGStateTree",
                "Slate",
                "SlateCore",
                "StateTreeModule"
            }
        );
    }
}