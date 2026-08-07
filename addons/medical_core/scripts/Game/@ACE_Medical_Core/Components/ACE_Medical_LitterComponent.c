//------------------------------------------------------------------------------------------------
class ACE_Medical_LitterComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Component for medical litter spawned by treatments
class ACE_Medical_LitterComponent : ScriptComponent
{
	protected static ACE_Medical_Core_Settings s_Settings;
	
	//------------------------------------------------------------------------------------------------
	//! Insert into garbage system for clean-up
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		ChimeraWorld world = owner.GetWorld();
		
		if (!s_Settings)
			s_Settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		
		if (s_Settings.m_fLitterCleanUpTime < 0)
			return;
		
		GarbageSystem system = GarbageSystem.Cast(world.FindSystem(GarbageSystem));
		if (system)
			system.Insert(owner, s_Settings.m_fLitterCleanUpTime);
	}
}
