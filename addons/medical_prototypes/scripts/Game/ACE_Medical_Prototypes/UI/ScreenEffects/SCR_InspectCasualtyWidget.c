//------------------------------------------------------------------------------------------------
//! TO DO: This is a mess to mod. Better come up with our own menu at some point
modded class SCR_InspectCasualtyWidget : SCR_InfoDisplayExtended
{
	// Maximum time in seconds for showing the window
	protected const float ACE_MEDICAL_MAX_SHOW_DURATION_S = 10;
	
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
			switch (bloodState)
			{
				case ACE_Medical_EBloodState.CLASS_1_HEMORRHAGE: { bloodText = "#ACE_Medical-BloodState_Class1"; break; }
				case ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE: { bloodText = "#ACE_Medical-BloodState_Class2"; break; }
				case ACE_Medical_EBloodState.CLASS_3_HEMORRHAGE: { bloodText = "#ACE_Medical-BloodState_Class3"; break; }
				case ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE: { bloodText = "#ACE_Medical-BloodState_Class4"; break; }
			}
			
			if (bleedingRate > 0)
				bloodText = bleedingIntensityText + "\n" + bloodText;
		}
		
		array<string> medicationTexts = {};
		
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(m_Target.FindComponent(ACE_Medical_MedicationComponent));
		if (medicationComponent)
		{
			array<float> times;
			array<string> messages;
			medicationComponent.GetLogData(times, messages);
			medicationTexts.Reserve(times.Count());
			
			for (int i = times.Count() - 1; i >= 0; i--)
			{
				int hours = Math.Floor(times[i]);
				int minutes = Math.Round(60 * (times[i] - hours));
				medicationTexts.Insert(string.Format("%1:%2 %3", hours, minutes, messages[i]));
			}
		}
		
		SCR_InventoryDamageInfoUI damageInfoUI = SCR_InventoryDamageInfoUI.Cast(m_wCasualtyInspectWidget.FindHandler(SCR_InventoryDamageInfoUI));
		if (damageInfoUI)
		{
			damageInfoUI.SetName(sName);
			damageInfoUI.SetDamageStateVisible(groupDamageIntensity, regenerating, damageIntensity, damageIntensityText);
			damageInfoUI.SetBleedingStateVisible(bloodState > ACE_Medical_EBloodState.NORMAL, bloodText);
			damageInfoUI.SetTourniquetStateVisible(isTourniquetted);
			damageInfoUI.SetSalineBagStateVisible(isSalineBagged);
			damageInfoUI.ACE_Medical_SetMedicationVisible(!medicationTexts.IsEmpty(), SCR_StringHelper.Join("\n", medicationTexts));
			damageInfoUI.SetFractureStateVisible(0, 0);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Override window showing time
	override void DisableWidget()
	{
		super.DisableWidget();
		m_fTimeTillClose = ACE_MEDICAL_MAX_SHOW_DURATION_S;
	}
}
