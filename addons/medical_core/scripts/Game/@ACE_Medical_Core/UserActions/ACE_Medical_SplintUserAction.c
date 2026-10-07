//------------------------------------------------------------------------------------------------
class ACE_Medical_SplintUserAction : SCR_MorphineUserAction
{
	[Attribute(defvalue: "#AR-Inventory_Bleeding", desc: "String for when target is bleeding")]
	protected LocalizedString m_sIsBleeding;

	[Attribute(defvalue: "Remove splint", desc: "String when splint is already applied")]
	protected LocalizedString m_sRemoveSplint;

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (ACE_Medical_IsSplinted())
			return true;

		return super.CanBeShownScript(user);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (ACE_Medical_IsSplinted())
			return true;

		ChimeraCharacter userCharacter = ChimeraCharacter.Cast(user);
		if (!userCharacter)
			return false;

		SCR_ConsumableItemComponent consumableComponent = GetConsumableComponent(userCharacter);
		if (!consumableComponent)
			return false;

		SCR_ConsumableEffectHealthItems effect = SCR_ConsumableEffectHealthItems.Cast(consumableComponent.GetConsumableEffect());
		if (!effect)
			return false;

		int reason;
		if (!effect.CanApplyEffectToHZ(GetOwner(), userCharacter, m_eHitZoneGroup, reason))
		{
			if (reason == SCR_EConsumableFailReason.UNDAMAGED)
				SetCannotPerformReason(m_sNotDamaged);
			else if (reason == SCR_EConsumableFailReason.ALREADY_APPLIED)
				SetCannotPerformReason(m_sAlreadyApplied);
			else if (reason == SCR_EConsumableFailReason.IS_BLEEDING)
				SetCannotPerformReason(m_sIsBleeding);

			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_CharacterDamageManagerComponent damageManager = ACE_Medical_GetDamageManager();
		if (!damageManager)
			return;

		if (ACE_Medical_IsSplinted())
		{
			ACE_Medical_RemoveSplintEffect(damageManager);
			damageManager.ACE_Medical_SetGroupSplinted(m_eHitZoneGroup, false);
			return;
		}

		super.PerformAction(pOwnerEntity, pUserEntity);

		damageManager.ACE_Medical_SetGroupSplinted(m_eHitZoneGroup, true);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (ACE_Medical_IsSplinted())
		{
			outName = m_sRemoveSplint;
			return true;
		}

		return super.GetActionNameScript(outName);
	}

	//------------------------------------------------------------------------------------------------
	protected bool ACE_Medical_IsSplinted()
	{
		SCR_CharacterDamageManagerComponent damageManager = ACE_Medical_GetDamageManager();
		if (!damageManager)
			return false;

		if (damageManager.ACE_Medical_GetGroupSplinted(m_eHitZoneGroup))
			return true;

		array<HitZone> groupHitZones = {};
		damageManager.GetHitZonesOfGroup(m_eHitZoneGroup, groupHitZones);

		return damageManager.IsDamageEffectPresentOnHitZones(ACE_Medical_SplintDamageEffect, groupHitZones);
	}

	//------------------------------------------------------------------------------------------------
	protected SCR_CharacterDamageManagerComponent ACE_Medical_GetDamageManager()
	{
		ChimeraCharacter ownerCharacter = ChimeraCharacter.Cast(GetOwner());
		if (!ownerCharacter)
			return null;

		return SCR_CharacterDamageManagerComponent.Cast(ownerCharacter.GetDamageManager());
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Medical_RemoveSplintEffect(SCR_CharacterDamageManagerComponent damageManager)
	{
		array<ref SCR_PersistentDamageEffect> splintEffects = {};
		damageManager.FindAllDamageEffectsOfType(ACE_Medical_SplintDamageEffect, splintEffects);

		if (splintEffects.IsEmpty())
			return;

		array<HitZone> groupHitZones = {};
		damageManager.GetHitZonesOfGroup(m_eHitZoneGroup, groupHitZones);

		foreach (SCR_PersistentDamageEffect splintEffect : splintEffects)
		{
			if (groupHitZones.Contains(splintEffect.GetAffectedHitZone()))
				damageManager.TerminateDamageEffect(splintEffect);
		}
	}
}