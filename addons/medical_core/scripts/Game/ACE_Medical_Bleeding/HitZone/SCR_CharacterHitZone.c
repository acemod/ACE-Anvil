//------------------------------------------------------------------------------------------------
modded class SCR_CharacterHitZone : SCR_RegeneratingHitZone
{
	//-----------------------------------------------------------------------------------------------------------
	//! Refactored calculation from SCR_BleedingDamageEffect::CalculateBleedingRate
	float ACE_Medical_CalculateBleedingRate()
	{
		return (1 - GetHealthScaled()) * GetMaxBleedingRate();
	}
}
