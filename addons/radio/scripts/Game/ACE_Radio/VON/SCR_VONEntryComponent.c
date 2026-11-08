//------------------------------------------------------------------------------------------------
//! Show non-default encryption key in radio radial menu
modded class SCR_VONEntryComponent : SCR_SelectionMenuEntryComponent
{
	protected TextWidget m_wACE_Radio_EncryptionKeyText;

	//------------------------------------------------------------------------------------------------
	void ACE_Radio_SetEncryptionKeyText(string text)
	{
		bool isVisible = !text.IsEmpty();
		m_wACE_Radio_EncryptionKeyText.SetVisible(isVisible);
		
		if (isVisible)
			m_wACE_Radio_EncryptionKeyText.SetText(text);
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		m_wACE_Radio_EncryptionKeyText = TextWidget.Cast(m_wRoot.FindAnyWidget("ACE_Radio_EncryptionKeyText"));
	}
}
