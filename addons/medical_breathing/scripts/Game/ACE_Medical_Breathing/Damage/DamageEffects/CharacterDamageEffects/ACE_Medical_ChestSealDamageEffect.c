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
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(dmgManager.GetOwner());
		if (!char)
			return;
		
		ACE_Medical_RespiratoryComponent respiratoryComponent = char.ACE_Medical_GetRespiratoryComponent();
		if (!respiratoryComponent)
			return;
		
		respiratoryComponent.SetPneumothoraxVolume(0);
	}
}
