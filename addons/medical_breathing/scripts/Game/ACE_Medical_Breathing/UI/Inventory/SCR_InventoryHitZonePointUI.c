//------------------------------------------------------------------------------------------------
modded class SCR_InventoryHitZonePointUI : ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! Add medication info to tooltip
	override protected void ACE_Medical_ShowGlobalStatTooltip(Widget w, ACE_Medical_EUIInfoType type)
	{
		super.ACE_Medical_ShowGlobalStatTooltip(w, type);
		
		if (!m_DamageInfo)
			return;
		
		if (type & ACE_Medical_EUIInfoType.PNEUMOTHORAX)
			m_DamageInfo.ACE_Medical_SetPneumothoraxStateVisible(true, ACE_Medical_GetPneumothoraxInfoText());
	}
	
	//------------------------------------------------------------------------------------------------
	string ACE_Medical_GetPneumothoraxInfoText()
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (!char)
			return "";
		
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(char.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitals)
			return "";
		
		string pneumothoraxText;
		if (vitals.HasTensionPneumothorax())
			return "#ACE_Medical-TensionPneumothorax";
		else if (vitals.GetPneumothoraxScale() > 0)
			return "#ACE_Medical-OpenPneumothorax";
		
		return "";
	}
}
