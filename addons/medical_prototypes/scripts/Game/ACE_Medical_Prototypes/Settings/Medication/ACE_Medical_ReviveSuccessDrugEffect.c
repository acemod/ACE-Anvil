//------------------------------------------------------------------------------------------------
class ACE_Medical_ReviveSuccessDrugEffect : ACE_Medical_HillDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(SCR_ChimeraCharacter target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = target.ACE_Medical_GetCardiovascularComponent();
		if (cardiovascularComponent)
			cardiovascularComponent.SetReviveSuccessCheckTimerScale(ComputeEffect(concentrations) + 1);
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(SCR_ChimeraCharacter target)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = target.ACE_Medical_GetCardiovascularComponent();
		if (cardiovascularComponent)
			cardiovascularComponent.SetReviveSuccessCheckTimerScale(1);
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override void OnDiag(SCR_ChimeraCharacter target, float timeSlice)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = target.ACE_Medical_GetCardiovascularComponent();
		if (cardiovascularComponent)
			DbgUI.Text(string.Format("- Revive success scale:    %1", cardiovascularComponent.GetReviveSuccessCheckTimerScale()));
	}
#endif
}
