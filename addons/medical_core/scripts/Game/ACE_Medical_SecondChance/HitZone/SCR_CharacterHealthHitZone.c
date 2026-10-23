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
		
		ACE_Medical_SecondChanceSystem system = ACE_Medical_SecondChanceSystem.GetInstance(damageManager.GetOwner().GetWorld());
		if (system)
		{
			SCR_CharacterHitZone struckHitZone = damageManager.ACE_Medical_GetLastStruckPhysicalHitZone();
			damageManager.AddBleedingEffectOnHitZone(struckHitZone);
			system.Register(damageManager, struckHitZone);
		}
		else
		{
			// Put kill on callqueue to avoid potential crash (e.g. when called from HitZone::OnDamageStateChanged)
			GetGame().GetCallqueue().Call(damageManager.Kill, damageManager.GetInstigator());
		}
	}
}
