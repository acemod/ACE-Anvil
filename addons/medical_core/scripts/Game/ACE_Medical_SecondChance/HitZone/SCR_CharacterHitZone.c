//------------------------------------------------------------------------------------------------
modded class SCR_CharacterHitZone : SCR_RegeneratingHitZone
{
	[Attribute(defvalue: "1", desc: "Probability for second chance for this hit zone", uiwidget: UIWidgets.Slider, params: "0 1")]
	protected float m_fACE_Medical_SecondChance;
	
	protected bool m_bACE_Medical_WasSecondChanceGranted = false;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Apply second chance setting
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings)
			m_fACE_Medical_SecondChance = settings.m_fDefaultSecondChance;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns true when second chance is granted by this hit zone
	bool ACE_Medical_ShouldGrantSecondChance()
	{
		//! Always regrant when previously granted
		if (m_bACE_Medical_WasSecondChanceGranted)
			return true;
		
		if (m_fACE_Medical_SecondChance < 1 && m_fACE_Medical_SecondChance <= Math.RandomFloat(0, 1))
			return false;
		
		m_bACE_Medical_WasSecondChanceGranted = true;
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Unsets previously granted second chance
	void ACE_Medical_ClearSecondChanceHistory()
	{
		m_bACE_Medical_WasSecondChanceGranted = false;
	}
}
