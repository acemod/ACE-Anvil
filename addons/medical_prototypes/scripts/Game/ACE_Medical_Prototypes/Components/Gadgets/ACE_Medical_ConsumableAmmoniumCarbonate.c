//! Epinephrine effect: Heals resilience hit zone
[BaseContainerProps()]
class ACE_Medical_ConsumableAmmoniumCarbonate : SCR_ConsumableEffectHealthItems
{
	[Attribute(defvalue: "0.2", desc: "Probability for applying effect when resilience recovery scale is close to 0")]
	protected float m_fSuccessChanceMin;
	
	[Attribute(defvalue: "1", desc: "Probability for applying effect when resilience recovery scale is at ACE_Medical_CardiovascularSystemComponent.m_fMaxRevivalResilienceRecoveryScale or above")]
	protected float m_fSuccessChanceMax;
	
	//------------------------------------------------------------------------------------------------
	//! Can always be applied in advanced medical
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Can always be applied in advanced medical
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Add chance for applying effect
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		ACE_Medical_CardiovascularSystemComponent cardiovascularSystem = ACE_Medical_CardiovascularSystemComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularSystemComponent));
		if (!cardiovascularSystem)
			return;
		
		float scale = cardiovascularSystem.GetResilienceRecoveryScale();
		if (scale <= 0)
			return;
		
		// TO DO: Add time of last attempt as a factor to prevent spamming to become meta
		if (Math.RandomFloat01() > Math.Map(scale, 0, cardiovascularSystem.GetMaxRevivalResilienceRecoveryScale(), m_fSuccessChanceMin, m_fSuccessChanceMax))
			return;
		
		super.ApplyEffect(target, user, item, animParams);
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableAmmoniumCarbonate()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_AMMONIUM_CARBONATE;
	}
}
