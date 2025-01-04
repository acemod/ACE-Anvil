modded class SCR_VONController: ScriptComponent
{
	const string ACE_SOUND_START_HIGH = "Sounds/ACE_UI/beep_high.wav";
	const string ACE_SOUND_START_LOW = "Sounds/ACE_UI/beep_low.wav";
	const string ACE_SOUND_END = "Sounds/ACE_UI/click_off.wav";

	protected ACE_ERadioBeep m_eBeepTypeCh1;
	protected ACE_ERadioBeep m_eBeepTypeCh2;

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
		BaseContainer settings = ACE_RadioSettingsModule.GetInstance();

		settings.Get(ACE_RadioSettingsModule.BEEPCH1, m_eBeepTypeCh1);
		settings.Get(ACE_RadioSettingsModule.BEEPCH2, m_eBeepTypeCh2);

		PrintFormat("SCR_VONController.AFM_LoadSettings: %1, %2", m_eBeepTypeCh1, m_eBeepTypeCh2, level: LogLevel.DEBUG);
	}

	//------------------------------------------------------------------------------------------------
	protected string AFM_GetBeepSound(int transreceiverNumber)
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
		super.SetActiveTransmit(entry);

		SCR_VONEntryRadio radioEntry = SCR_VONEntryRadio.Cast(entry);
		if (!radioEntry)
			return;

		string beep = AFM_GetBeepSound(radioEntry.GetTransceiverNumber());
		if (beep != "")
			// TODO use sound event SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_RADIO_TURN_OFF);
			AudioSystem.PlaySound(beep);
	}

	//------------------------------------------------------------------------------------------------
	override void DeactivateVON(EVONTransmitType transmitType = EVONTransmitType.NONE)
	{
		if (m_bIsActive)
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
