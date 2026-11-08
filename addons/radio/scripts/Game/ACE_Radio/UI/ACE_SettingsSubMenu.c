modded class ACE_SettingsSubMenu : SCR_SettingsSubMenuBase
{
	//------------------------------------------------------------------------------------------------
	override void OnTabCreate(Widget menuRoot, ResourceName buttonsLayout, int index)
	{
		BaseContainer radioSettings = ACE_Radio_SettingsModule.GetInstance();
		VerticalLayoutWidget wRadioRoot = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("ACE_Radio"));

		super.OnTabCreate(menuRoot, buttonsLayout, index);

		if (!radioSettings || !wRadioRoot)
		{
			PrintFormat("Unable to load ACE_Radio Radio settings: %1, %2", radioSettings, wRadioRoot, level: LogLevel.ERROR);
			return;
		}

		// Radio beep Ch1
		SCR_SelectionWidgetComponent checkBoxBeep1 = SCR_SelectionWidgetComponent.GetSelectionComponent("Beep_Ch1", wRadioRoot);
		if (checkBoxBeep1)
		{
			int value;
			radioSettings.Get(ACE_Radio_SettingsModule.BEEPCH1, value);

			checkBoxBeep1.SetCurrentItem(value >> 1, false, false); //--- Shift the value, because it's a flag
			checkBoxBeep1.m_OnChanged.Insert(SetBeepCh1);
		}
		else
		{
			Print("Radio setting 'Beep_Ch1' not found", LogLevel.WARNING);
		}

		// Radio beep Ch2
		SCR_SelectionWidgetComponent checkBoxBeep2 = SCR_SelectionWidgetComponent.GetSelectionComponent("Beep_Ch2", wRadioRoot);
		if (checkBoxBeep2)
		{
			int value;
			radioSettings.Get(ACE_Radio_SettingsModule.BEEPCH2, value);

			checkBoxBeep2.SetCurrentItem(value >> 1, false, false); //--- Shift the value, because it's a flag
			checkBoxBeep2.m_OnChanged.Insert(SetBeepCh2);
		}
		else
		{
			Print("Radio setting 'Beep_Ch2' not found", LogLevel.WARNING);
		}

		// Cycle transreceiver beep
		SCR_SelectionWidgetComponent checkBoxCycle = SCR_SelectionWidgetComponent.GetSelectionComponent("Beep_Cycle", wRadioRoot);
		if (checkBoxCycle)
		{
			bool value;
			radioSettings.Get(ACE_Radio_SettingsModule.BEEPCYCLE, value);

			checkBoxCycle.SetCurrentItem(value, false, false);
			checkBoxCycle.m_OnChanged.Insert(SetBeepCycle);
		}
		else
		{
			Print("Radio setting 'Beep_Cycle' not found", LogLevel.WARNING);
		}

		// Transmission end click off
		SCR_SelectionWidgetComponent checkBoxClick = SCR_SelectionWidgetComponent.GetSelectionComponent("Click_Off", wRadioRoot);
		if (checkBoxClick)
		{
			bool value;
			radioSettings.Get(ACE_Radio_SettingsModule.CLICKOFF, value);

			checkBoxClick.SetCurrentItem(value, false, false);
			checkBoxClick.m_OnChanged.Insert(SetClickOff);
		}
		else
		{
			Print("Radio setting 'Click_Off' not found", LogLevel.WARNING);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void SetBeepCh1(SCR_SelectionWidgetComponent checkBox, int state)
	{
		SetBeepProp(checkBox, state, ACE_Radio_SettingsModule.BEEPCH1);
	}

	//------------------------------------------------------------------------------------------------
	protected void SetBeepCh2(SCR_SelectionWidgetComponent checkBox, int state)
	{
		SetBeepProp(checkBox, state, ACE_Radio_SettingsModule.BEEPCH2);
	}

	//------------------------------------------------------------------------------------------------
	protected void SetBeepProp(SCR_SelectionWidgetComponent checkBox, int state, string prop)
	{
		PrintFormat("SetBeepProp: %1, %2", prop, state, level: LogLevel.NORMAL);

		state = 1 << state; //--- Shift the value, because it's a flag

		ACE_Radio_SettingsModule.GetInstance().Set(prop, state);
		GetGame().UserSettingsChanged();
	}

	//------------------------------------------------------------------------------------------------
	protected void SetBeepCycle(SCR_SelectionWidgetComponent checkBox, bool state)
	{
		PrintFormat("SetBeepCycle: %1", state, level: LogLevel.NORMAL);

		ACE_Radio_SettingsModule.GetInstance().Set(ACE_Radio_SettingsModule.BEEPCYCLE, state);
		GetGame().UserSettingsChanged();
	}

	//------------------------------------------------------------------------------------------------
	protected void SetClickOff(SCR_SelectionWidgetComponent checkBox, bool state)
	{
		PrintFormat("SetClickOff: %1", state, level: LogLevel.NORMAL);

		ACE_Radio_SettingsModule.GetInstance().Set(ACE_Radio_SettingsModule.CLICKOFF, state);
		GetGame().UserSettingsChanged();
	}
}
