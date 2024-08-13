//------------------------------------------------------------------------------------------------	
modded class SCR_InspectCasualtyWidget : SCR_InfoDisplayExtended
{
	//------------------------------------------------------------------------------------------------
	//! Gather and update data of target character into widget
	override void UpdateWidgetData()
	{
		if (!m_Target || !m_wCasualtyInspectWidget)
			return;
		
		string sName;
		GetCasualtyName(sName, m_Target);
	
		float bleedingRate;
		int groupDamageIntensity;
		bool regenerating, isTourniquetted, isSalineBagged, isMorphined;
		string damageIntensity, damageIntensityText, bleedingIntensityText;
		array <bool> hZGroupsBleeding = {};
		
		SCR_InventoryHitZonePointUI hitZonePointUI = new SCR_InventoryHitZonePointUI();
		GetDamageInfo(hitZonePointUI, m_Target, bleedingRate, hZGroupsBleeding, groupDamageIntensity, regenerating, isTourniquetted, isSalineBagged, isMorphined);
		hitZonePointUI.GetDamageInfoTexts(EHitZoneGroup.VIRTUAL, groupDamageIntensity, bleedingRate, damageIntensity, damageIntensityText, bleedingIntensityText);
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
		ACE_Medical_EBloodState bloodState = damageManager.GetBloodHitZone().GetDamageState();
		string bloodText;
		
		if (bloodState > ACE_Medical_EBloodState.NORMAL)
		{
			bloodText = SCR_Enum.GetEnumName(ACE_Medical_EBloodState, bloodState);
			
			if (bleedingRate > 0)
			bloodText += "\n(" + bleedingIntensityText + ")";
		}
		
		SCR_InventoryDamageInfoUI damageInfoUI = SCR_InventoryDamageInfoUI.Cast(m_wCasualtyInspectWidget.FindHandler(SCR_InventoryDamageInfoUI));
		if (damageInfoUI)
		{
			damageInfoUI.SetName(sName);
			damageInfoUI.SetDamageStateVisible(groupDamageIntensity, regenerating, damageIntensity, damageIntensityText);
			damageInfoUI.SetBleedingStateVisible(bloodState > ACE_Medical_EBloodState.NORMAL, bloodText);
			damageInfoUI.SetTourniquetStateVisible(isTourniquetted);
			damageInfoUI.SetSalineBagStateVisible(isSalineBagged);
			damageInfoUI.SetMorphineStateVisible(isMorphined);
			damageInfoUI.SetFractureStateVisible(0, 0);
		}
	}
}
