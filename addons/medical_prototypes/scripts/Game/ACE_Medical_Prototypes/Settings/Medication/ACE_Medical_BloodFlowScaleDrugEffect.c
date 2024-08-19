//------------------------------------------------------------------------------------------------
class ACE_Medical_BloodFlowScaleDrugEffect : ACE_Medical_HillDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(IEntity target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager)
			damageManager.ACE_Medical_SetBloodFlowScale(1 - ComputeEffect(concentrations));
	}
}
