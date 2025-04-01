//------------------------------------------------------------------------------------------------
class ACE_Medical_BloodFlowScaleDrugEffect : ACE_Medical_HillDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(SCR_ChimeraCharacter target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.GetDamageManager());
		if (damageManager)
			damageManager.ACE_Medical_SetBloodFlowScale(1 - ComputeEffect(concentrations));
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(SCR_ChimeraCharacter target)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.GetDamageManager());
		if (damageManager)
			damageManager.ACE_Medical_SetBloodFlowScale(1);
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override void OnDiag(SCR_ChimeraCharacter target, float timeSlice)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.GetDamageManager());
		if (damageManager)
			DbgUI.Text(string.Format("- Blood flow scale:        %1", damageManager.ACE_Medical_GetBloodFlowScale()));
	}
#endif
}
