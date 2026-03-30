//------------------------------------------------------------------------------------------------
modded class SCR_WeaponInfoWidgets
{
	Widget m_wACE_Zeroing;
	RichTextWidget m_wACE_ZeroingText;
	RichTextWidget m_wACE_ZeroingSuffix;

	//------------------------------------------------------------------------------------------------
	override bool Init(Widget root)
	{
		super.Init(root);
		m_wACE_Zeroing = root.FindAnyWidget("m_ACE_Zeroing");
		m_wACE_ZeroingText = RichTextWidget.Cast(root.FindAnyWidget("m_ACE_ZeroingText"));
		m_wACE_ZeroingSuffix = RichTextWidget.Cast(root.FindAnyWidget("m_ACE_ZeroingSuffix"));
		return true;
	}
}
