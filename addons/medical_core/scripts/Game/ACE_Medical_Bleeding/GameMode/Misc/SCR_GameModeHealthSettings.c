//------------------------------------------------------------------------------------------------
modded class SCR_GameModeHealthSettings : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (!settings)
			return;
		
		m_fDOTScale = settings.m_fBleedingRateScale;
		m_fRegenScale = settings.m_fBloodRegenScale;
	}
}
