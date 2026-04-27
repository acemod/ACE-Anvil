//------------------------------------------------------------------------------------------------
//! User action used to remove an active splint from a limb.
class ACE_Medical_RemoveSplintUserAction : ScriptedUserAction
{
	[Attribute(uiwidget: UIWidgets.ComboBox, desc: "Hit zone group", enums: ParamEnumArray.FromEnum(ECharacterHitZoneGroup))]
	protected ECharacterHitZoneGroup m_eHitZoneGroup;

	[Attribute(defvalue: "Remove splint", desc: "Action name")]
	protected LocalizedString m_sActionName;

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return ACE_Medical_HasSplint();
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		return ACE_Medical_HasSplint();
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter targetCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		if (!targetCharacter)
			return;

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(targetCharacter.GetDamageManager());
		if (!damageManager)
			return;

		array<ref SCR_PersistentDamageEffect> splintEffects = {};
		damageManager.FindAllDamageEffectsOfType(ACE_Medical_SplintDamageEffect, splintEffects);

		array<HitZone> groupHitZones = {};
		damageManager.GetHitZonesOfGroup(m_eHitZoneGroup, groupHitZones);

		foreach (SCR_PersistentDamageEffect splintEffect : splintEffects)
		{
			if (groupHitZones.Contains(splintEffect.GetAffectedHitZone()))
				damageManager.TerminateDamageEffect(splintEffect);
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = m_sActionName;
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool ACE_Medical_HasSplint()
	{
		SCR_ChimeraCharacter targetCharacter = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!targetCharacter)
			return false;

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(targetCharacter.GetDamageManager());
		if (!damageManager)
			return false;

		array<HitZone> groupHitZones = {};
		damageManager.GetHitZonesOfGroup(m_eHitZoneGroup, groupHitZones);

		return damageManager.IsDamageEffectPresentOnHitZones(ACE_Medical_SplintDamageEffect, groupHitZones);
	}
}