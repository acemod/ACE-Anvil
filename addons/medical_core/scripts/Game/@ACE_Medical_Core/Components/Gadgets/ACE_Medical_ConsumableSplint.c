//------------------------------------------------------------------------------------------------
//! Splint effect: Heals limbs
[BaseContainerProps()]
class ACE_Medical_ConsumableSplint : SCR_ConsumableBandage
{
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(target);
		if (!char)
			return false;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!damageManager)
			return false;
		
		if (damageManager.GetGroupDamageOverTime(group, EDamageType.BLEEDING) > 0)
		{
			failReason = SCR_EConsumableFailReason.IS_BLEEDING;
			return false;
		}
		
		array<HitZone> groupHitZones = {};
		damageManager.GetHitZonesOfGroup(group, groupHitZones);
		
		if (!HasHealableHitZone(groupHitZones))
		{
			failReason = SCR_EConsumableFailReason.UNDAMAGED;
			return false;
		}
		
		if (damageManager.IsDamageEffectPresentOnHitZones(ACE_Medical_SplintDamageEffect, groupHitZones))
		{
			failReason = SCR_EConsumableFailReason.ALREADY_APPLIED;
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool HasHealableHitZone(array<HitZone> groupHitZones)
	{
		foreach (HitZone hitZone : groupHitZones)
		{
			if (hitZone.GetHealthScaled() < 0.667)
				return true;
		}
		
		return false;
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableSplint()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_SPLINT;
	}
}
