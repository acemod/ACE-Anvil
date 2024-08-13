//-----------------------------------------------------------------------------------------------------------
class ACE_Medical_BrainHitZone : SCR_RegeneratingHitZone
{
	protected ACE_Medical_CardiovascularSystemComponent m_pCardiovascularSystem;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Store instance on SCR_CharacterDamageManagerComponent on init
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		if (damageManager)
			damageManager.ACE_Medical_SetBrainHitZone(this);
		
		m_pCardiovascularSystem = ACE_Medical_CardiovascularSystemComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_CardiovascularSystemComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! TO DO: Figure out where the additional regeneration is comming from so we actually don't need this
	//! Do not regenerate when in cardiac arrest
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float damage = super.ComputeEffectiveDamage(damageContext, isDOT);
		
		if (damage < 0 && m_pCardiovascularSystem.IsInCardiacArrest())
			return 0;
		
		return damage;
	}
}
