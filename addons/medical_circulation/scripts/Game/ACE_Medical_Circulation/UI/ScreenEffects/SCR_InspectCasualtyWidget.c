//------------------------------------------------------------------------------------------------
modded class SCR_InspectCasualtyWidget : SCR_InfoDisplayExtended
{
	//------------------------------------------------------------------------------------------------
	//! Gather and update data of target character into widget
	override void UpdateWidgetData()
	{
		super.UpdateWidgetData();
		
		if (!m_Target || !m_wCasualtyInspectWidget)
			return;
		
		SCR_InventoryDamageInfoUI damageInfoUI = SCR_InventoryDamageInfoUI.Cast(m_wCasualtyInspectWidget.FindHandler(SCR_InventoryDamageInfoUI));
		if (!damageInfoUI)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;
		
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
		}
		
		damageInfoUI.ACE_Medical_SetBloodStateVisible(bloodState > ACE_Medical_EBloodState.NORMAL, bloodText);
	}
}
