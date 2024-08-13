//------------------------------------------------------------------------------------------------
modded class SCR_CharacterResilienceHitZone : SCR_RegeneratingHitZone
{
	protected ACE_Medical_CardiovascularSystemComponent m_pACE_Medical_CardiovascularSystem;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_CardiovascularSystem = ACE_Medical_CardiovascularSystemComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_CardiovascularSystemComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Apply recovery modifiers from cardiovascular system
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float damage = super.ComputeEffectiveDamage(damageContext, isDOT);
		
		if (damageContext.damageType == EDamageType.REGENERATION)
			damage *= m_pACE_Medical_CardiovascularSystem.GetResilienceRecoveryScale();

		return damage;
	}
}
