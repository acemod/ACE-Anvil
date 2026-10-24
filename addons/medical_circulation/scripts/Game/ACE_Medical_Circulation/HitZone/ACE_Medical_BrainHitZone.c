//-----------------------------------------------------------------------------------------------------------
class ACE_Medical_BrainHitZone : SCR_RegeneratingHitZone
{
	protected ACE_Medical_VitalsComponent m_pVitalsComponent;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Store instance on SCR_CharacterDamageManagerComponent on init
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		if (damageManager)
			damageManager.ACE_Medical_SetBrainHitZone(this);
		
		m_pVitalsComponent = ACE_Medical_VitalsComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_VitalsComponent));
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Kill character when destroyed
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer() || (newState != EDamageState.DESTROYED))
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (damageManager)
			damageManager.Kill(damageManager.GetInstigator());
	}
	
	//------------------------------------------------------------------------------------------------
	//! TO DO: Figure out where the additional regeneration is comming from so we actually don't need this
	//! Do not regenerate when in cardiac arrest
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float damage = super.ComputeEffectiveDamage(damageContext, isDOT);
		
		if (damage < 0 && (m_pVitalsComponent.GetVitalStateID() & (ACE_Medical_EVitalStateID.CARDIAC_ARREST | ACE_Medical_EVitalStateID.RESUSCITATION)))
			return 0;
		
		return damage;
	}
}
