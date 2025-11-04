//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_IKestrelDataScreen : ACE_Weather_IKestrelScreen
{
	[Attribute(defvalue: "{3262679C50EF4F01}UI/Textures/Icons/icons_wrapperUI.imageset", desc: "Name of the icon in the imageset", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "imageset")]
	ResourceName m_sIconImageSet;
	
	[Attribute(desc: "Name of the icon in the imageset")]
	protected string m_sIconName;
	
	[Attribute(desc: "Rotation of the icon in degrees")]
	protected float m_fIconRotation;
	
	[Attribute(desc: "String for title")]
	protected string m_sTitle;
	
	[Attribute(desc: "String for unit")]
	protected string m_sUnit;
	
	[Attribute(desc: "Strings for footer", uiwidget: UIWidgets.EditBoxMultiline)]
	protected string m_sFooterText;
	
	//------------------------------------------------------------------------------------------------
	override void OnOpen(Widget root)
	{
		super.OnOpen(root);
		
		Widget displayData = root.FindAnyWidget("DisplayData");
		displayData.SetVisible(true);
		
		// Hide all widgets in body, such that overrides only have to enable those they need
		ACE_WidgetTools.SetDirectChildrenVisible(displayData.FindAnyWidget("Body"), false);
		
		ImageWidget icon = ImageWidget.Cast(displayData.FindAnyWidget("Icon"));
		bool shouldBeVisible = !m_sIconImageSet.IsEmpty() && !m_sIconName.IsEmpty();
		icon.SetVisible(shouldBeVisible);
		
		if (shouldBeVisible)
		{
			icon.LoadImageFromSet(0, m_sIconImageSet, m_sIconName);
			icon.SetRotation(m_fIconRotation);
		}	
		
		TextWidget title = TextWidget.Cast(displayData.FindAnyWidget("Title"));
		title.SetText(m_sTitle);
		
		TextWidget unit = TextWidget.Cast(displayData.FindAnyWidget("Unit"));
		unit.SetVisible(!m_sUnit.IsEmpty());
		unit.SetText(m_sUnit);
		
		RichTextWidget footer = RichTextWidget.Cast(displayData.FindAnyWidget("Footer"));
		shouldBeVisible = !m_sFooterText.IsEmpty();
		footer.SetVisible(shouldBeVisible);
		
		if (shouldBeVisible)
			footer.SetText(m_sFooterText);
	}
}
