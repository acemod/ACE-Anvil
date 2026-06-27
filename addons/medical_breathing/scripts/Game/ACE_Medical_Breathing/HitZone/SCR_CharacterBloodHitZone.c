//-----------------------------------------------------------------------------------------------------------
modded class SCR_CharacterBloodHitZone : SCR_RegeneratingHitZone
{
	protected float m_fACE_Medical_HemeConcentration = 0.0098; // mmol / ml
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns blood volume in ml
	//! Health of blood hit zone in "ACE Medical Circulations" is only 3000 ml
	//! to reflect the death treshold at 50% blood loss
	float ACE_Medical_GetBloodVolume()
	{
		return 3000 + GetHealth();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns arterial blood volume in ml
	float ACE_Medical_GetArterialBloodVolume()
	{
		return 0.3 * ACE_Medical_GetBloodVolume();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns venous blood volume in ml
	float ACE_Medical_GetVenousBloodVolume()
	{
		return 0.7 * ACE_Medical_GetBloodVolume();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	float ACE_Medical_GetHemeConcentration()
	{
		return m_fACE_Medical_HemeConcentration;
	}
}
