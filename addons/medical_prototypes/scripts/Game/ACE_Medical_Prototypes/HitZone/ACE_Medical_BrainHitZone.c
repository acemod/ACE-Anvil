//-----------------------------------------------------------------------------------------------------------
class ACE_Medical_BrainHitZone : SCR_RegeneratingHitZone
{
	protected ACE_Medical_CardiovascularComponent m_pCardiovascularComponent;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Store instance on SCR_CharacterDamageManagerComponent on init
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		if (!ownerChar)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		if (damageManager)
			damageManager.ACE_Medical_SetBrainHitZone(this);
		
		m_pCardiovascularComponent = ownerChar.ACE_Medical_GetCardiovascularComponent();
	}
	
	//------------------------------------------------------------------------------------------------
	//! TO DO: Figure out where the additional regeneration is comming from so we actually don't need this
	//! Do not regenerate when in cardiac arrest
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float damage = super.ComputeEffectiveDamage(damageContext, isDOT);
		
		if (damage < 0 && m_pCardiovascularComponent.IsInCardiacArrest())
			return 0;
		
		return damage;
	}
}
