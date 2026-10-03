using UnrealBuildTool;
public class OneLifeEditorTarget : TargetRules
{
    public OneLifeEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        ExtraModuleNames.Add("OneLife");
    }
}