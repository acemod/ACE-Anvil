//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_IKestrelRefHeadingScreen : ACE_Weather_IKestrelScreen
{
	[Attribute()]
	protected string m_sCommandNavText;
	
	[Attribute()]
	protected string m_sPowerNavText;
	
	protected int m_iTmpRefHeading;
	protected TextWidget m_wHeading;
	protected Widget m_wOptions;
	protected TextWidget m_wInstructions;
	
	//------------------------------------------------------------------------------------------------
	override void OnOpen(Widget root)
	{
		super.OnOpen(root);
		Widget display = root.FindAnyWidget("DisplayRefHeading");
		display.SetVisible(true);
		m_wHeading = TextWidget.Cast(display.FindAnyWidget("Heading"));
		m_wOptions = display.FindAnyWidget("Options");
		m_wInstructions = TextWidget.Cast(display.FindAnyWidget("Instructions"));
		
		Widget navigation = display.FindAnyWidget("Navigation");
		TextWidget command = TextWidget.Cast(navigation.FindAnyWidget("Command"));
		command.SetText(m_sCommandNavText);
		TextWidget power = TextWidget.Cast(navigation.FindAnyWidget("Power"));
		power.SetText(m_sPowerNavText);
		m_wHeading.SetText(string.Format("%1°", m_pKestrel.GetRefHeading().ToString(3)));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnButtonClickServer(ACE_EGadgetButtonID buttonID)
	{
		if (super.OnButtonClickServer(buttonID))
			return true;
		
		switch (buttonID)
		{
			case ACE_EGadgetButtonID.POWER:
			{
				m_pKestrel.PopScreen();
				return true;
			};
		}
		
		return false;
	}
}
