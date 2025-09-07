modded class SCR_VONController: ScriptComponent
{
	const string ACE_SOUND_START_HIGH = "Sounds/ACE_Radio/beep_high.wav";
	const string ACE_SOUND_START_LOW = "Sounds/ACE_Radio/beep_low.wav";
	const string ACE_SOUND_END = "Sounds/ACE_Radio/click_off.wav";

	protected ACE_ERadioBeep m_eBeepTypeCh1;
	protected ACE_ERadioBeep m_eBeepTypeCh2;
	protected bool m_bBeepCycle;
	protected bool m_bClickOff;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		ACE_LoadSettings();
		GetGame().OnUserSettingsChangedInvoker().Insert(ACE_LoadSettings);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_LoadSettings()
	{
		BaseContainer settings = ACE_Radio_SettingsModule.GetInstance();

		settings.Get(ACE_Radio_SettingsModule.BEEPCH1, m_eBeepTypeCh1);
		settings.Get(ACE_Radio_SettingsModule.BEEPCH2, m_eBeepTypeCh2);
		settings.Get(ACE_Radio_SettingsModule.BEEPCYCLE, m_bBeepCycle);
		settings.Get(ACE_Radio_SettingsModule.CLICKOFF, m_bClickOff);

		PrintFormat("SCR_VONController.ACE_LoadSettings: %1, %2, %3, %4",
			m_eBeepTypeCh1, 
			m_eBeepTypeCh2,
			m_bBeepCycle,
			m_bBeepCycle,
			level: LogLevel.DEBUG);
	}

	//------------------------------------------------------------------------------------------------
	protected string ACE_GetBeepSound(int transreceiverNumber)
	{
		ACE_ERadioBeep beepType;
		if (transreceiverNumber == 1)
			beepType = m_eBeepTypeCh1;
		else
			beepType = m_eBeepTypeCh2;

		switch (beepType) {
			case ACE_ERadioBeep.LOW:
				return ACE_SOUND_START_LOW;
			case ACE_ERadioBeep.NORMAL:
				return ACE_SOUND_START_HIGH;
		}

		return "";
	}

	//------------------------------------------------------------------------------------------------
	override void SetActiveTransmit(notnull SCR_VONEntry entry)
	{
		if (m_ActiveEntry != entry && !m_bBeepCycle)
		{
			super.SetActiveTransmit(entry);
			return;
		}
		
		super.SetActiveTransmit(entry);

		SCR_VONEntryRadio radioEntry = SCR_VONEntryRadio.Cast(entry);
		if (!radioEntry)
			return;

		string beep = ACE_GetBeepSound(radioEntry.GetTransceiverNumber());
		if (beep != "")
			// TODO use sound event SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_RADIO_TURN_OFF);
			AudioSystem.PlaySound(beep);
	}

	//------------------------------------------------------------------------------------------------
	override void DeactivateVON(EVONTransmitType transmitType = EVONTransmitType.NONE)
	{
		Pring("SCR_VONController.ACE_LoadSettings: m_bClickOff", m_bClickOff);
		if (m_bIsActive && m_bClickOff)
		{
			if (transmitType != EVONTransmitType.DIRECT)
			{
				// TODO use sound event SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_RADIO_TURN_OFF);
				AudioSystem.PlaySound(ACE_SOUND_END);
			}
		};

		super.DeactivateVON(transmitType);
	}
}
