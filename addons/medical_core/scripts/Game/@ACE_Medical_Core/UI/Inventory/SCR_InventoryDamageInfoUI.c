//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/UI/Inventory", description: "Inventory Item Info UI class")]
modded class SCR_InventoryDamageInfoUI : ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! Fix broken vanilla implementation for inspect casualty widget
	//! i.e. handle case where m_wFractureIcon2Widget does not exist
	override void SetFractureStateVisible(bool armFractured, bool legFractured)
	{
		super.SetFractureStateVisible(armFractured, legFractured);
		
		// if m_wFractureIcon2Widget exists, it's already properly handled by vanilla
		if (m_wFractureIcon2Widget || !m_wFractureIconWidget || !m_wFractureTextWidget)
			return;
		
		m_wFractureIconWidget.LoadImageFromSet(0, m_sMedicalIconsImageSet, m_sFractureIcon);
		m_wFractureIconWidget.SetVisible(armFractured || legFractured);
		
		string text;
		if (armFractured)
			text += m_sArmFractureText;
		
		if (armFractured && legFractured)
			text += "\n";
		
		if (legFractured)
			text += m_sLegFractureText;

		m_wFractureTextWidget.SetText(text);
		m_wFractureTextWidget.SetVisible(armFractured || legFractured);
	}
}
