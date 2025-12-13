//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	override void ACE_Medical_UpdateResilienceRegenScale()
	{
		if (m_ACE_Medical_Vitals && !m_ACE_Medical_Vitals.CanBreath())
		{
			m_fACE_Medical_ResilienceRegenScale = 0;
			return;
		}
		
		super.ACE_Medical_UpdateResilienceRegenScale();
	}
}
