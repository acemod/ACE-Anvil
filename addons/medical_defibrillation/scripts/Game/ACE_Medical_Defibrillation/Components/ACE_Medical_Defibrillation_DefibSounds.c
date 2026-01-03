class ACE_Medical_Defibrillation_DefibSounds
{
	const static string SOUNDANALYSING = "ACE_Medical_Defibrillation_SoundAnalysing";
	const static string SOUNDCHARGED = "ACE_Medical_Defibrillation_SoundCharged";
	const static string SOUNDCHARGING = "ACE_Medical_Defibrillation_SoundCharging";
	const static string SOUNDCONNECTED = "ACE_Medical_Defibrillation_SoundConnected";
	const static string SOUNDDISCONNECTED = "ACE_Medical_Defibrillation_SoundDisconnected";
	const static string SOUNDNOSHOCKADVISED = "ACE_Medical_Defibrillation_SoundNoShockAdvised";
	const static string SOUNDSHOCKADVISED = "ACE_Medical_Defibrillation_SoundShockAdvised";
	const static string SOUNDSHOCKTHUMP = "ACE_Medical_Defibrillation_SoundThump";
	const static string SOUNDCPRBEEP = "ACE_Medical_Defibrillation_SoundCPRBeep";
	const static string SOUNDCHARGEDBEEPLOW = "ACE_Medical_Defibrillation_SoundChargedBeepLow";
	const static string SOUNDCHARGEDBEEPHIGH = "ACE_Medical_Defibrillation_SoundChargedBeepHigh";
	const static string SOUNDCPRREMINDER = "ACE_Medical_Defibrillation_SoundCPRReminder";
	const static string SOUNDDONOTTOUCHPATIENT = "ACE_Medical_Defibrillation_SoundDoNotTouchPatient";
	
	float m_fLastCPRPaceTimer;
	float m_fContinueCPRTimer;
	float m_fPatientTouchTimer;
	float m_fChargedBeepTimer;
	float m_iChargedBeepPhase;
	
	void ACE_Medical_Defibrillation_DefibSounds()
	{
	}
}