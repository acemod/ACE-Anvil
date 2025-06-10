//------------------------------------------------------------------------------------------------
modded class SCR_GameModeHealthSettings : ScriptComponent
{
	[Attribute(defvalue: "-1", uiwidget: UIWidgets.Slider, desc: "Maximum possibe bleeding rate in ml/s (Negative value means unlimited)", category: "ACE Medical")]
	protected float m_fACE_Medical_MaxTotalBleedingRate;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (!settings)
			return;
		
		m_fDOTScale = settings.m_fBleedRateScale;
		m_fRegenScale = settings.m_fBloodRegenScale;
		m_fACE_Medical_MaxTotalBleedingRate = settings.m_fMaxTotalBleedingRate;
	}
	
	//------------------------------------------------------------------------------------------------
	float ACE_Medical_GetMaxTotalBleedingRate()
	{
		return m_fACE_Medical_MaxTotalBleedingRate;
	}
}
