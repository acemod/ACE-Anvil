//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiacArrestDamageEffect: SCR_DotDamageEffect
{
	[Attribute(defvalue: "0.333", desc: "Damage the brain takes per second")]
	protected float m_fBrainDamageRate;
	
	[Attribute(defvalue: "10", desc: "Delay in seconds before the effect starts [s]")]
	protected float m_fEffectStartDelayS;
	protected float m_fDelayTimerS = 0;
		
	//------------------------------------------------------------------------------------------------
	protected override void HandleConsequences(SCR_ExtendedDamageManagerComponent dmgManager, DamageEffectEvaluator evaluator)
	{
		super.HandleConsequences(dmgManager, evaluator);

		evaluator.HandleEffectConsequences(this, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Make sure this effect can only be added once
	event override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (!dmgManager)
			return true;
		
		ACE_Medical_BrainHitZone hitZone = charDamageManager.ACE_Medical_GetBrainHitZone();
		if (!hitZone)
			return true;
		
		SetAffectedHitZone(hitZone);
		array<ref SCR_PersistentDamageEffect> effects = {};
		charDamageManager.FindAllDamageEffectsOfTypeOnHitZone(ACE_Medical_CardiacArrestDamageEffect, hitZone, effects);
		return !effects.IsEmpty();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectAdded(dmgManager);
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		
		SetDamageType(GetDefaultDamageType());
		SetDPS(m_fBrainDamageRate);
		SetMaxDuration(0);
		SetInstigator(charDamageManager.GetInstigator());

		// Terminate any regeneration
		array<ref SCR_PersistentDamageEffect> effects = {};
		charDamageManager.FindAllDamageEffectsOfTypeOnHitZone(SCR_PassiveHitZoneRegenDamageEffect, GetAffectedHitZone(), effects);
		foreach (SCR_PersistentDamageEffect effect : effects)
		{
			effect.Terminate();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnEffectRemoved(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectAdded(dmgManager);
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		
		IEntity target = charDamageManager.GetOwner();
		if (!target)
			return;
		
		// Add regneration when no longer in cardiac arrest
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (cardiovascularComponent && !cardiovascularComponent.IsInCardiacArrest())
			charDamageManager.RegenVirtualHitZone(SCR_RegeneratingHitZone.Cast(GetAffectedHitZone()));
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		m_fDelayTimerS += timeSlice;

		if (m_fDelayTimerS < m_fEffectStartDelayS)
			return;
		
		DealDot(timeSlice, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.ACE_MEDICAL_HYPOXIA;
	}
}
