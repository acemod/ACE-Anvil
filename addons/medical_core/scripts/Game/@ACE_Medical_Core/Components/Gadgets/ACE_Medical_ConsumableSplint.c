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
		
		if (!HasHealableHitZone(groupHitZones, damageManager.ACE_Medical_GetSplintMaxHealScaled()))
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
	protected bool HasHealableHitZone(array<HitZone> groupHitZones, float maxHealScaled)
	{
		foreach (HitZone hitZone : groupHitZones)
		{
			if (hitZone.GetHealthScaled() < maxHealScaled)
				return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override float GetItemRegenSpeed()
	{
		float regenSpeed = super.GetItemRegenSpeed();
		
		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (settings)
			regenSpeed *= settings.m_fSplintHealingRateScale;
		
		return regenSpeed;
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableSplint()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_SPLINT;
	}
}
