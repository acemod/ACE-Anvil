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
		
		ChimeraCharacter character = ChimeraCharacter.Cast(m_Target);
		if (!character)
			return;
		
		SCR_CharacterDamageManagerComponent damageMan = SCR_CharacterDamageManagerComponent.Cast(character.GetDamageManager());
		if (!damageMan)
			return;
		
		damageInfoUI.ACE_CBRN_SetParalysisStateVisible(damageMan.ACE_CBRN_HasParalysis());
	}
}
