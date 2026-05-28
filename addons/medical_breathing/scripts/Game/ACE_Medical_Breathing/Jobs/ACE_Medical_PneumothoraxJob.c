//------------------------------------------------------------------------------------------------
class ACE_Medical_PneumothoraxJob : ACE_IFrameJob
{
	protected ref ACE_Medical_CharacterContext m_pContext;
	
	protected static ACE_Medical_Breathing_Settings s_Settings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_PneumothoraxJob()
	{
		if (!s_Settings)
			s_Settings = ACE_SettingsHelperT<ACE_Medical_Breathing_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		super.OnUpdate(timeSlice);
		
		if (!m_pContext.IsValid())
			return;
		
		// Can only deteriorate when breathing
		if (m_pContext.m_pVitals.GetRespiratoryRate() == 0)
			return;
		
		if (!m_pContext.m_pVitals.HasTensionPneumothorax() && Math.RandomFloat(0, 1) <  s_Settings.m_fDeteriorateToTensionPneumothoraxChancePerMinute)
			m_pContext.m_pVitals.SetHasTensionPneumothorax(true);
		
		if (Math.RandomFloat(0, 1) >= s_Settings.m_fPneumothoraxDeteriorationChancePerMinute)
			return;
		
		float nextScale = m_pContext.m_pVitals.GetPneumothoraxScale() + s_Settings.m_fPneumothoraxScaleDeteriorationStep;
		if (nextScale >= s_Settings.m_fMaxPneumothoraxScale)
		{
			nextScale = s_Settings.m_fMaxPneumothoraxScale;
			
			if (s_Settings.m_fPneumothoraxArrestEnabled)
				m_pContext.m_pVitals.SetVitalStateID(ACE_Medical_EVitalStateID.CARDIAC_ARREST);
			
			Stop();
		}
		
		m_pContext.m_pVitals.SetPneumothoraxScale(nextScale);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetContext(ACE_Medical_CharacterContext context)
	{
		m_pContext = context;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_CharacterContext GetContext()
	{
		return m_pContext;
	}
}
