//------------------------------------------------------------------------------------------------
//! Add handling for vital hit zones
modded class SCR_CharacterHitZone : SCR_RegeneratingHitZone
{
	[Attribute(defvalue: "false", desc: "Whether the hit zone is vital and will kill the character when destroyed")]
	protected bool m_bACE_Medical_IsVital;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Vital hit zones trigger registration at ACE_Medical_SecondChanceSystem when destroyed
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer() || newState != EDamageState.DESTROYED)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return;
		
		//! Add bleeding to physical hit zones that get destroyed
		damageManager.AddBleedingEffectOnHitZone(this);
		
		if (!m_bACE_Medical_IsVital)
			return;
		
		ACE_Medical_SecondChanceSystem system = ACE_Medical_SecondChanceSystem.GetInstance();
		if (system)
			system.Register(damageManager, this);
		else
			damageManager.Kill(damageManager.GetInstigator());
	}
}
