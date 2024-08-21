//------------------------------------------------------------------------------------------------
class ACE_Medical_ReviveSuccessDrugEffect : ACE_Medical_HillDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(IEntity target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (cardiovascularComponent)
			cardiovascularComponent.SetReviveSuccessCheckTimerScale(ComputeEffect(concentrations) + 1);
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(IEntity target)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (cardiovascularComponent)
			cardiovascularComponent.SetReviveSuccessCheckTimerScale(1);
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override void OnDiag(IEntity target, float timeSlice)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (cardiovascularComponent)
			DbgUI.Text(string.Format("- Revive success scale:    %1", cardiovascularComponent.GetReviveSuccessCheckTimerScale()));
	}
#endif
}
