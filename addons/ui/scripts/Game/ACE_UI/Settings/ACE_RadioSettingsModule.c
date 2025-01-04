enum ACE_ERadioBeep
{
	OFF		= 1 << 0,
	NORMAL	= 1 << 1,
	LOW		= 1 << 2,
}

class ACE_RadioSettingsModule: ModuleGameSettings
{
	[Attribute(ACE_ERadioBeep.NORMAL.ToString(), uiwidget: UIWidgets.ComboBox, desc: "Sound to play when activating VoN Ch1", enums: ParamEnumArray.FromEnum(ACE_ERadioBeep))]
	ACE_ERadioBeep m_eBeepCh1;

	[Attribute(ACE_ERadioBeep.LOW.ToString(), uiwidget: UIWidgets.ComboBox, desc: "Sound to play when activating VoN Ch2", enums: ParamEnumArray.FromEnum(ACE_ERadioBeep))]
	ACE_ERadioBeep m_eBeepCh2;
}
