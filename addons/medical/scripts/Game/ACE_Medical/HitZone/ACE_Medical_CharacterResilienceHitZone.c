//------------------------------------------------------------------------------------------------
modded class SCR_CharacterResilienceHitZone : SCR_RegeneratingHitZone
{
	//------------------------------------------------------------------------------------------------
	//! Multiply regen DPS by the regen scale
	override float CalculatePassiveRegenDPS(bool considerRegenScale = true)
	{
		float regenDPS = super.CalculatePassiveRegenDPS(considerRegenScale);
		
		const SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (damageManager)
			regenDPS *= damageManager.GetResilienceRegenScale();	
		
		return regenDPS;
	}
}
