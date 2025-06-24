//-----------------------------------------------------------------------------------------------------------
//! health hitzone - Receives damage from physical hitzones
modded class SCR_CharacterHealthHitZone : SCR_HitZone
{
	//-----------------------------------------------------------------------------------------------------------
	//! Register at ACE_Medical_SecondChanceSystem when destroyed
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer())
			return;
		
		if (newState != EDamageState.DESTROYED)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return;
		
		ACE_Medical_SecondChanceSystem system = ACE_Medical_SecondChanceSystem.GetInstance();
		if (system)
			system.Register(damageManager, damageManager.ACE_Medical_GetLastStruckPhysicalHitZone());
		else
			damageManager.Kill(damageManager.GetInstigator());
	}
}
