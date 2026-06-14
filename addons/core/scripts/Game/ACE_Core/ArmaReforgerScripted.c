//------------------------------------------------------------------------------------------------
//! Main game instance.
modded class ArmaReforgerScripted : ChimeraGame
{
	//------------------------------------------------------------------------------------------------
	override protected void OnMissionSet(MissionHeader mission)
	{
		super.OnMissionSet(mission);
		
		if (Replication.IsServer())
			ACE_SettingsSystem.OnMissionSet(mission);
	}
}
