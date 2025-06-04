//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected ACE_Medical_Settings m_pACE_Medical_Settings;
	protected SCR_CharacterHitZone m_pACE_Medical_LastStruckPhysicalHitZone;
	protected float m_fACE_Medical_ResilienceRegenScale = 1;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize members
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		m_pACE_Medical_Settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		m_fACE_Medical_ResilienceRegenScale = m_pACE_Medical_Settings.m_fDefaultResilienceRegenScale;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Update last struck physical hit zone
	override void OnDamage(notnull BaseDamageContext damageContext)
	{
		super.OnDamage(damageContext);
		
		if (!Replication.IsServer())
			return;
		
		SCR_CharacterHitZone struckPhysicalHitZone = SCR_CharacterHitZone.Cast(damageContext.struckHitZone);
		if (struckPhysicalHitZone)
			m_pACE_Medical_LastStruckPhysicalHitZone = struckPhysicalHitZone;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns last stuck physical hit zone
	SCR_CharacterHitZone ACE_Medical_GetLastStruckPhysicalHitZone()
	{
		return m_pACE_Medical_LastStruckPhysicalHitZone;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_UpdateResilienceRegenScale()
	{
		if (m_pBloodHitZone.GetDamageState() == ECharacterBloodState.UNCONSCIOUS)
			m_fACE_Medical_ResilienceRegenScale = 0;
		else
			m_fACE_Medical_ResilienceRegenScale = m_pACE_Medical_Settings.m_fDefaultResilienceRegenScale;
	}
	
	//------------------------------------------------------------------------------------------------
	float ACE_Medical_GetResilienceRegenScale()
	{
		return m_fACE_Medical_ResilienceRegenScale;
	}
}
