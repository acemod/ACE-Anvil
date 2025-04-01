//------------------------------------------------------------------------------------------------
class ACE_Medical_SystemicVascularResistencDrugEffect : ACE_Medical_LinearDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(SCR_ChimeraCharacter target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = target.ACE_Medical_GetCardiovascularComponent();
		if (cardiovascularComponent)
			cardiovascularComponent.SetSystemicVascularResistenceMedicationAdjustment(ComputeEffect(concentrations));
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(SCR_ChimeraCharacter target)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = target.ACE_Medical_GetCardiovascularComponent();
		if (cardiovascularComponent)
			cardiovascularComponent.SetSystemicVascularResistenceMedicationAdjustment(0);
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override void OnDiag(SCR_ChimeraCharacter target, float timeSlice)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = target.ACE_Medical_GetCardiovascularComponent();
		if (cardiovascularComponent)
			DbgUI.Text(string.Format("- SVR:                     %1", cardiovascularComponent.GetSystemicVascularResistenceMedicationAdjustment()));
	}
#endif
}
