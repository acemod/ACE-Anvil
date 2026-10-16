//------------------------------------------------------------------------------------------------
class ACE_Medical_DamageInfoEntryUIComponent : ScriptedWidgetComponent
{
	protected static const ResourceName MEDICAL_ICONS_IMAGE_SET = "{B9199157B90D6216}UI/Textures/InventoryIcons/Medical/Medical-icons.imageset";

	protected Widget m_wRoot;
	protected ImageWidget m_wIcon;
	protected TextWidget m_wText;

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		m_wIcon = ImageWidget.Cast(w.FindAnyWidget("Icon"));
		m_wText = TextWidget.Cast(w.FindAnyWidget("Text"));
		Hide();
	}

	//------------------------------------------------------------------------------------------------
	void SetState(bool visible, string iconName, string text, string additionalText = "")
	{
		if (!visible || (text.IsEmpty() && additionalText.IsEmpty()))
		{
			Hide();
			return;
		}

		if (m_wIcon)
		{
			m_wIcon.SetVisible(!iconName.IsEmpty());
			if (!iconName.IsEmpty())
				m_wIcon.LoadImageFromSet(0, MEDICAL_ICONS_IMAGE_SET, iconName);
		}

		if (m_wText)
		{
			string label = WidgetManager.Translate(text);
			if (!additionalText.IsEmpty())
			{
				if (!label.IsEmpty())
					label += "\n";

				label += WidgetManager.Translate(additionalText);
			}

			m_wText.SetText(label);
		}

		m_wRoot.SetVisible(true);
	}

	//------------------------------------------------------------------------------------------------
	void Hide()
	{
		if (m_wRoot)
			m_wRoot.SetVisible(false);
	}
}
