//-----------------------------------------------------------------------------------------------------------
//! health hitzone - Receives damage from physical hitzones
modded class SCR_CharacterHealthHitZone : SCR_HitZone
{
	protected SCR_CharacterControllerComponent m_pACE_Medical_CharacterCtrl;
	protected bool m_bACE_Medical_IsPlayer;
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_CharacterCtrl = SCR_CharacterControllerComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterControllerComponent));
		m_bACE_Medical_IsPlayer = EntityUtils.IsPlayer(pOwnerEntity);
	}

	//-----------------------------------------------------------------------------------------------------------
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float effDamage = super.ComputeEffectiveDamage(damageContext, isDOT);
		
		if (!m_bACE_Medical_IsPlayer || m_pACE_Medical_CharacterCtrl.IsUnconscious())
			return effDamage;
		
		float remainingHealth = GetHealth() - damageContext.damageValue;
		
		if (remainingHealth > 0)
			return effDamage;
		
		return damageContext.damageValue - remainingHealth - 1;
	}
}