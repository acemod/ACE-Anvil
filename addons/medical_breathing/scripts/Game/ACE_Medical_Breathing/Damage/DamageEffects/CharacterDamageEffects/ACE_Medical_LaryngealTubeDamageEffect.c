//------------------------------------------------------------------------------------------------
class ACE_Medical_LaryngealTubeDamageEffect : SCR_InstantDamageEffect
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
		SCR_ChimeraCharacter targetChar = SCR_ChimeraCharacter.Cast(dmgManager.GetOwner());
		if (!targetChar)
			return;
		
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(targetChar.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitalsComponent)
			return;
		
		vitalsComponent.SetIsAirwayOccluded(false);
		vitalsComponent.SetIsAirwayObstructed(false);
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(targetChar.GetDamageManager());
		if (damageManager)
			damageManager.ACE_Medical_UpdateResilienceRegenScale();
	}
}
