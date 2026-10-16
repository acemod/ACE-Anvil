//------------------------------------------------------------------------------------------------
class ACE_Medical_DamageInfoUIComponent : ScriptedWidgetComponent
{
	protected Widget m_wRoot;
	protected ACE_Medical_DamageInfoEntryUIComponent m_DamageInfo;
	protected ACE_Medical_DamageInfoEntryUIComponent m_BleedingInfo;
	protected ACE_Medical_DamageInfoEntryUIComponent m_TourniquetInfo;
	protected ACE_Medical_DamageInfoEntryUIComponent m_SalineBagInfo;
	protected ACE_Medical_DamageInfoEntryUIComponent m_ImpairmentInfo;
	protected ref SCR_InventoryHitZonePointUI m_HitZonePointUI = new SCR_InventoryHitZonePointUI();

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		m_DamageInfo = GetConditionInfo("DamageInfo");
		m_BleedingInfo = GetConditionInfo("BleedingInfo");
		m_TourniquetInfo = GetConditionInfo("TourniquetInfo");
		m_SalineBagInfo = GetConditionInfo("SalineBagInfo");
		m_ImpairmentInfo = GetConditionInfo("ImpairmentInfo");
	}

	//------------------------------------------------------------------------------------------------
	protected ACE_Medical_DamageInfoEntryUIComponent GetConditionInfo(string widgetName)
	{
		if (!m_wRoot)
			return null;

		Widget widget = m_wRoot.FindAnyWidget(widgetName);
		if (!widget)
			return null;

		return ACE_Medical_DamageInfoEntryUIComponent.Cast(widget.FindHandler(ACE_Medical_DamageInfoEntryUIComponent));
	}

	//------------------------------------------------------------------------------------------------
	protected bool GetHitZoneGroupState(SCR_CharacterDamageManagerComponent damageManager, ECharacterHitZoneGroup hitZoneGroup, out float damage, out float bleedingRate, out bool isTourniquetted)
	{
		damage = 0;
		bleedingRate = 0;
		isTourniquetted = false;

		array<HitZone> hitZones = {};
		damageManager.GetHitZonesOfGroupFromOwner(hitZoneGroup, hitZones);

		float totalHealthScaled;
		int hitZoneCount;
		foreach (HitZone hitZone : hitZones)
		{
			if (!SCR_CharacterHitZone.Cast(hitZone))
				continue;

			totalHealthScaled += hitZone.GetHealthScaled();
			hitZoneCount++;
		}

		// GetGroupHealthScaled returns zero for absent groups, which would imply a false injury.
		if (hitZoneCount == 0)
			return false;

		damage = Math.Clamp(1 - totalHealthScaled / hitZoneCount, 0, 1);
		bleedingRate = Math.Max(damageManager.GetGroupDamageOverTime(hitZoneGroup, EDamageType.BLEEDING), 0);
		isTourniquetted = damageManager.GetGroupTourniquetted(hitZoneGroup);
		if (isTourniquetted)
			bleedingRate *= damageManager.GetTourniquetStrengthMultiplier();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool UpdateDamage(SCR_CharacterDamageManagerComponent damageManager, ECharacterHitZoneGroup hitZoneGroup)
	{
		float damage, bleedingRate;
		bool isTourniquetted;
		if (!damageManager || hitZoneGroup == ECharacterHitZoneGroup.VIRTUAL || !GetHitZoneGroupState(damageManager, hitZoneGroup, damage, bleedingRate, isTourniquetted))
		{
			Reset();
			return false;
		}

		int damageIntensity;
		float health = 1 - damage;
		if (health < m_HitZonePointUI.m_fLowDamageThreshold)
			damageIntensity = 4;
		else if (health < m_HitZonePointUI.m_fMediumDamageThreshold)
			damageIntensity = 3;
		else if (health < m_HitZonePointUI.m_fHighDamageThreshold)
			damageIntensity = 2;
		else if (damage > 0)
			damageIntensity = 1;

		string damageIcon, damageText, bleedingText;
		m_HitZonePointUI.GetDamageInfoTexts(hitZoneGroup, damageIntensity, bleedingRate, damageIcon, damageText, bleedingText);
		bool isRegenerating = damageIntensity > 0 && (damageManager.GetGroupDamageOverTime(hitZoneGroup, EDamageType.HEALING) != 0 || damageManager.GetGroupDamageOverTime(hitZoneGroup, EDamageType.REGENERATION) != 0);
		string regenerationText;
		if (isRegenerating)
			regenerationText = "#AR-DamageInfo_Regeneration";

		if (damageIntensity == 0)
			damageText = "";

		if (m_DamageInfo)
			m_DamageInfo.SetState(damageIntensity > 0, damageIcon, damageText, regenerationText);
		if (m_BleedingInfo)
			m_BleedingInfo.SetState(bleedingRate > 0, "Blood_UI", bleedingText);
		if (m_TourniquetInfo)
			m_TourniquetInfo.SetState(isTourniquetted, "Tourniquet_UI", "#AR-DamageInfo_TourniquetApplied");
		if (m_SalineBagInfo)
			m_SalineBagInfo.SetState(damageManager.GetGroupSalineBagged(hitZoneGroup), "Saline-bag_UI", "#AR-DamageInfo_SalineBagApplied");
		if (m_ImpairmentInfo)
		{
			string impairmentText;
			if (hitZoneGroup == ECharacterHitZoneGroup.LEFTARM || hitZoneGroup == ECharacterHitZoneGroup.RIGHTARM)
				impairmentText = "#AR_Inventory_ArmsDamage";
			else if (hitZoneGroup == ECharacterHitZoneGroup.LEFTLEG || hitZoneGroup == ECharacterHitZoneGroup.RIGHTLEG)
				impairmentText = "#AR_Inventory_LegsDamage";

			m_ImpairmentInfo.SetState(IsHitZoneGroupImpaired(damageManager, hitZoneGroup, damage), "Bone-Fracture_UI", impairmentText);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsHitZoneGroupImpaired(SCR_CharacterDamageManagerComponent damageManager, ECharacterHitZoneGroup hitZoneGroup, float damage)
	{
		// Shared arm/leg debuffs must not mark a healthy limb on the opposite side.
		if (damage <= 0)
			return false;

		switch (hitZoneGroup)
		{
			case ECharacterHitZoneGroup.LEFTARM:
			case ECharacterHitZoneGroup.RIGHTARM:
			{
				return damageManager.GetAimingDamage() > 0;
			}

			case ECharacterHitZoneGroup.LEFTLEG:
			case ECharacterHitZoneGroup.RIGHTLEG:
			{
				return damageManager.GetMovementDamage() > 0;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	void Reset()
	{
		if (m_DamageInfo)
			m_DamageInfo.Hide();
		if (m_BleedingInfo)
			m_BleedingInfo.Hide();
		if (m_TourniquetInfo)
			m_TourniquetInfo.Hide();
		if (m_SalineBagInfo)
			m_SalineBagInfo.Hide();
		if (m_ImpairmentInfo)
			m_ImpairmentInfo.Hide();
	}
}
