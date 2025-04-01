//------------------------------------------------------------------------------------------------
modded class SCR_CharacterResilienceHitZone : SCR_RegeneratingHitZone
{
	protected ACE_Medical_CardiovascularComponent m_pACE_Medical_CardiovascularComponent;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		if (!ownerChar)
			return;
		
		m_pACE_Medical_CardiovascularComponent = ownerChar.ACE_Medical_GetCardiovascularComponent();
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
