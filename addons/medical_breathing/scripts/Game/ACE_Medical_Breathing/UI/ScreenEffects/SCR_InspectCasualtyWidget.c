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
		
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(m_Target.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitals)
			return;
		
		string pneumothoraxText;
		if (vitals.HasTensionPneumothorax())
			pneumothoraxText = "#ACE_Medical-TensionPneumothorax";
		else if (vitals.GetPneumothoraxScale() > 0)
			pneumothoraxText = "#ACE_Medical-OpenPneumothorax";
		
		damageInfoUI.ACE_Medical_SetPneumothoraxStateVisible(!pneumothoraxText.IsEmpty(), pneumothoraxText);
	}
}
