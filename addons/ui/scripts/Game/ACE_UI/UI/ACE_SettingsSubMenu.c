modded class ACE_SettingsSubMenu : SCR_SettingsSubMenuBase
{
	const string ACE_BEEPCH1_PROP = "m_eBeepCh1";
	const string ACE_BEEPCH2_PROP = "m_eBeepCh2";
	
	//------------------------------------------------------------------------------------------------
	override void OnTabCreate(Widget menuRoot, ResourceName buttonsLayout, int index)
	{
		BaseContainer radioSettings = GetRadioSettings();
		VerticalLayoutWidget wRadioRoot = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("ACE_UI_Radio"));

		super.OnTabCreate(menuRoot, buttonsLayout, index);

		if (!radioSettings || !wRadioRoot)
		{
			PrintFormat("Unable to load ACE_UI Radio settings: %1, %2", radioSettings, wRadioRoot, level: LogLevel.ERROR);
			return;
		}

		// Radio beep Ch1
		SCR_SelectionWidgetComponent checkBoxBeep1 = SCR_SelectionWidgetComponent.GetSelectionComponent("Beep_Ch1", wRadioRoot);
		if (checkBoxBeep1)
		{
			int value;
			radioSettings.Get(ACE_BEEPCH1_PROP, value);

			checkBoxBeep1.SetCurrentItem(value >> 1, false, false); //--- Shift the value, because it's a flag
			checkBoxBeep1.m_OnChanged.Insert(SetBeepCh1);
		}
		else
		{
			Print("Radio setting 'Beep_Ch1' not found", LogLevel.WARNING);
		}

		// Radio beep Ch1
		SCR_SelectionWidgetComponent checkBoxBeep2 = SCR_SelectionWidgetComponent.GetSelectionComponent("Beep_Ch2", wRadioRoot);
		if (checkBoxBeep2)
		{
			int value;
			radioSettings.Get(ACE_BEEPCH2_PROP, value);

			checkBoxBeep2.SetCurrentItem(value >> 1, false, false); //--- Shift the value, because it's a flag
			checkBoxBeep2.m_OnChanged.Insert(SetBeepCh2);
		}
		else
		{
			Print("Radio setting 'Beep_Ch2' not found", LogLevel.WARNING);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void SetBeepCh1(SCR_SelectionWidgetComponent checkBox, int state)
	{
		SetBeepProp(checkBox, state, ACE_BEEPCH1_PROP);
	}

	//------------------------------------------------------------------------------------------------
	protected void SetBeepCh2(SCR_SelectionWidgetComponent checkBox, int state)
	{
		SetBeepProp(checkBox, state, ACE_BEEPCH2_PROP);
	}

	//------------------------------------------------------------------------------------------------
	protected void SetBeepProp(SCR_SelectionWidgetComponent checkBox, int state, string prop)
	{
		PrintFormat("SetBeepProp: %1, %2", prop, state, level: LogLevel.NORMAL);
		
		state = 1 << state; //--- Shift the value, because it's a flag

		GetRadioSettings().Set(prop, state);
		GetGame().UserSettingsChanged();
	}
	
	//------------------------------------------------------------------------------------------------
	protected BaseContainer GetRadioSettings()
	{
		return GetGame().GetGameUserSettings().GetModule("ACE_RadioSettingsModule");
	}
}
