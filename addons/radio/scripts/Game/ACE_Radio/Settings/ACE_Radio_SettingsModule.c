enum ACE_ERadioBeep
{
	OFF		= 1 << 0,
	NORMAL	= 1 << 1,
	LOW		= 1 << 2,
}

class ACE_Radio_SettingsModule: ModuleGameSettings
{
	const string BEEPCH1 = "m_eBeepCh1";
	const string BEEPCH2 = "m_eBeepCh2";
	const string BEEPCYCLE = "m_bBeepCycle";
	
	[Attribute(ACE_ERadioBeep.NORMAL.ToString(), uiwidget: UIWidgets.ComboBox, desc: "Sound to play when activating VoN Ch1", enums: ParamEnumArray.FromEnum(ACE_ERadioBeep))]
	ACE_ERadioBeep m_eBeepCh1;

	[Attribute(ACE_ERadioBeep.LOW.ToString(), uiwidget: UIWidgets.ComboBox, desc: "Sound to play when activating VoN Ch2", enums: ParamEnumArray.FromEnum(ACE_ERadioBeep))]
	ACE_ERadioBeep m_eBeepCh2;
	
	[Attribute("1", desc: "Should radio channel beep sound play when cycling transreceivers")]
	bool m_bBeepCycle;
	
	//------------------------------------------------------------------------------------------------
	static BaseContainer GetInstance()
	{
		return GetGame().GetGameUserSettings().GetModule("ACE_Radio_SettingsModule");
	}
}
