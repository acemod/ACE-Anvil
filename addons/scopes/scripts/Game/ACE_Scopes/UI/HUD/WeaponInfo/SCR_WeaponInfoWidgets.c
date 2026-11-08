//------------------------------------------------------------------------------------------------
modded class SCR_WeaponInfoWidgets
{
	Widget m_wACE_Zeroing;
	RichTextWidget m_wACE_ZeroingText;
	RichTextWidget m_wACE_ZeroingSuffix;
	
	protected static const string ACE_SCOPES_LAYOUT_NAME = "{94AC490313D961C8}UI/layouts/HUD/WeaponInfo/ACE_Scopes_WeaponInfo.layout";

	//------------------------------------------------------------------------------------------------
	override bool Init(Widget root)
	{
		super.Init(root);
		GetGame().GetWorkspace().CreateWidgets(ACE_SCOPES_LAYOUT_NAME, m_wZeroing);
		m_wACE_Zeroing = root.FindAnyWidget("m_ACE_Zeroing");
		m_wACE_ZeroingText = RichTextWidget.Cast(root.FindAnyWidget("m_ACE_ZeroingText"));
		m_wACE_ZeroingSuffix = RichTextWidget.Cast(root.FindAnyWidget("m_ACE_ZeroingSuffix"));
		return true;
	}
}
