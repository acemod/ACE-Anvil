//-----------------------------------------------------------------------------------------------------------
modded class ACE_Medical_SecondChanceSystem : GameSystem
{
	//------------------------------------------------------------------------------------------------
	//! Add registration for organ hit zones
	void Register(notnull SCR_CharacterDamageManagerComponent damageManager, ACE_Medical_OrganHitZone struckHitZone)
	{
		// Refuse registration and kill character if second chance is not granted
		if (!struckHitZone || !damageManager.ACE_Medical_IsSecondChanceEnabled() || damageManager.ACE_Medical_ShouldDeactivateSecondChance() || !struckHitZone.ACE_Medical_ShouldGrantSecondChance())
		{
			damageManager.Kill(damageManager.GetInstigator());
			return;
		}
		
		damageManager.ACE_Medical_OnSecondChanceGranted();
		Register(damageManager);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check each destroyed organ hit zone if it grants second chance
	//! If not, kill the character
	override protected void HandleSecondChanceDeactivation(SCR_CharacterDamageManagerComponent damageManager)
	{
		super.HandleSecondChanceDeactivation(damageManager);
		
		if (!damageManager.ACE_Medical_IsSecondChanceEnabled())
			return;
	
		
		array<HitZone> hitZones = {};
		damageManager.GetAllHitZones(hitZones);
		foreach (HitZone hitZone : hitZones)
		{
			ACE_Medical_OrganHitZone organHitZone = ACE_Medical_OrganHitZone.Cast(hitZone);
			if (!organHitZone)
				continue;
			
			if (organHitZone.GetDamageState() != EDamageState.DESTROYED)
				continue;
			
			if (!organHitZone.ACE_Medical_ShouldGrantSecondChance())
			{
				damageManager.Kill(damageManager.GetInstigator());
				break;
			}
			
			ApplySecondChanceRegenToHitZone(damageManager, organHitZone);
		}
	}
}
