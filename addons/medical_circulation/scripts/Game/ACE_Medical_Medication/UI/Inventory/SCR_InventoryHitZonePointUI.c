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
		
		if (type & ACE_Medical_EUIInfoType.MEDICATION)
			m_DamageInfo.ACE_Medical_SetMedicationVisible(true, ACE_Medical_GetMedicationInfoText());
	}
	
	//------------------------------------------------------------------------------------------------
	string ACE_Medical_GetMedicationInfoText()
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (!char)
			return "";
		
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(char.FindComponent(ACE_Medical_MedicationComponent));
		if (!medicationComponent)
			return "";
		
		return medicationComponent.GetInfoText();
	}
}
