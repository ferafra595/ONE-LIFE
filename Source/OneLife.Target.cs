using UnrealBuildTool;
public class OneLifeTarget : TargetRules
{
    public OneLifeTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        ExtraModuleNames.Add("OneLife");
    }
}