//------------------------------------------------------------------------------------------------
modded class SCR_InspectCasualtyWidget : SCR_InfoDisplayExtended
{
	// Maximum time in seconds for showing the window
	protected const float ACE_MEDICAL_MAX_SHOW_DURATION_S = 10;
	
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
		
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(m_Target.FindComponent(ACE_Medical_MedicationComponent));
		if (!medicationComponent)
			return;
		
		string medicationText = medicationComponent.GetInfoText();
		damageInfoUI.ACE_Medical_SetMedicationVisible(!medicationText.IsEmpty(), medicationText);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Override window showing time
	override void DisableWidget()
	{
		super.DisableWidget();
		m_fTimeTillClose = ACE_MEDICAL_MAX_SHOW_DURATION_S;
	}
}
