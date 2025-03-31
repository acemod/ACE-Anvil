//------------------------------------------------------------------------------------------------
modded class SCR_CharacterResilienceHitZone : SCR_RegeneratingHitZone
{
	protected ACE_Medical_CardiovascularComponent m_pACE_Medical_CardiovascularComponent;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_CardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_CardiovascularComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Apply recovery modifiers from cardiovascular system
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float damage = super.ComputeEffectiveDamage(damageContext, isDOT);
		
		if (damageContext.damageType == EDamageType.REGENERATION)
			damage *= m_pACE_Medical_CardiovascularComponent.GetResilienceRecoveryScale();

		return damage;
	}
}
