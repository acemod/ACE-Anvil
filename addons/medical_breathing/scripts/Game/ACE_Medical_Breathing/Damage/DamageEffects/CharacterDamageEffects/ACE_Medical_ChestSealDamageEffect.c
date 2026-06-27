//------------------------------------------------------------------------------------------------
class ACE_Medical_ChestSealDamageEffect : SCR_InstantDamageEffect
{
	//------------------------------------------------------------------------------------------------
	protected override void HandleConsequences(SCR_ExtendedDamageManagerComponent dmgManager, DamageEffectEvaluator evaluator)
	{
		super.HandleConsequences(dmgManager, evaluator);
		evaluator.HandleEffectConsequences(this, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		IEntity target = dmgManager.GetOwner();
		if (!target)
			return;
		
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(target.FindComponent(ACE_Medical_VitalsComponent));
		if (vitalsComponent)
			vitalsComponent.SetPneumothoraxScale(0);
	}
}
