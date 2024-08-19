//------------------------------------------------------------------------------------------------
class ACE_Medical_HeartRateEffect : ACE_Medical_LinearDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(IEntity target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (cardiovascularComponent)
			cardiovascularComponent.SetHeartRateMedicationAdjustment(ComputeEffect(concentrations));
	}
}
