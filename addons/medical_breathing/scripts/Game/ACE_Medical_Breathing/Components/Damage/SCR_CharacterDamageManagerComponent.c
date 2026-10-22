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
	
	//-----------------------------------------------------------------------------------------------------------
	//! Unregister from medical systems when killed
	override void ACE_Medical_OnKilled()
	{
		super.ACE_Medical_OnKilled();
		
		ACE_Medical_PneumothoraxSystem system = ACE_Medical_PneumothoraxSystem.GetInstance(GetOwner().GetWorld());
		if (system)
			system.Unregister(SCR_ChimeraCharacter.Cast(GetOwner()));
		
		if (m_ACE_Medical_Vitals)
		{
			m_ACE_Medical_Vitals.SetRespiratoryRate(0);
			m_ACE_Medical_Vitals.SetSpO2(0);
		}
	}
}
