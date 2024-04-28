//------------------------------------------------------------------------------------------------
//! Hard-override of SCR_BandageUserAction
class SCR_BandageUserAction : SCR_HealingUserAction
{
	
	//------------------------------------------------------------------------------------------------
	//! Copy of vanilla method
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;
		
		CharacterControllerComponent controller = character.GetCharacterController();
		if (!controller)
			return;
		
		if (controller.GetLifeState() != ECharacterLifeState.ALIVE)
			return;
		
		SCR_ConsumableItemComponent consumableComponent = GetConsumableComponent(character);
		if (consumableComponent)
			consumableComponent.SetAlternativeModel(false);
	}

	//------------------------------------------------------------------------------------------------
	//! We move the handling from CanBeShownScript to CanBePerformedScript
	//! We check the bleeding via the array of all bleeding hit zones, which should be more reliable
	//! than the vanilla implementation
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		// Target character
		ChimeraCharacter char = ChimeraCharacter.Cast(GetOwner());
		if (!char)
			return false;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!damageManager)
			return false;
		
		array<HitZone> bleedingHitZones = {};
		damageManager.GetBleedingHitZones(bleedingHitZones);
		
		foreach (HitZone hitZone : bleedingHitZones)
		{
			SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(hitZone);
			if (!charHitZone)
				continue;
			
			if (charHitZone.GetHitZoneGroup() == m_eHitZoneGroup)
				return true;
		}

		SetCannotPerformReason(m_sNotBleeding);
		return false;
	}
}
