using UnrealBuildTool;
public class OneLife : ModuleRules
{
    public OneLife(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core","CoreUObject","Engine","InputCore","EnhancedInput",
            "UMG","Slate","SlateCore","AIModule","NavigationSystem",
            "GameplayTasks","StateTreeModule","GameplayStateTreeModule",
            "ChaosVehicles"
        });
    }
}