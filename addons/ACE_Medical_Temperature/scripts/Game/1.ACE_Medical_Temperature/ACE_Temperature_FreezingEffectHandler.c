class ACE_Temperature_FreezingEffectHandler :SCR_InfoDisplayExtended
{
	
	protected float m_fDeathProgress;
	protected bool m_bEnabled;
	protected ACE_Medical_VitalsComponent m_PlayerVitals;
	protected float m_fDefaultCoreTemperature;
	protected float m_fUnconTemperature;
	ImageWidget m_wImageWidget;
	void InitEffect(SCR_ChimeraCharacter char,Widget root){
		m_PlayerVitals=ACE_Medical_VitalsComponent.Cast(char.FindComponent(ACE_Medical_VitalsComponent));
		m_wImageWidget=ImageWidget.Cast(root.FindAnyWidget("ACE_Temperature_Freezing"));
		m_fDeathProgress=0;
		m_fDefaultCoreTemperature=ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings().m_fDefaultCoreTemperature;
		m_fUnconTemperature=ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings().m_CriticalThresholds.m_fTemperatureThreshold;
	}
	
	void EnableEffect(bool enable)
	{
		m_wImageWidget.SetEnabled(enable);
		m_wImageWidget.SetVisible(enable);
		m_bEnabled=enable;
	}
	
	void UpdateEffect(float timeSlice)
	{
		m_fDeathProgress=Math.InverseLerp(m_fDefaultCoreTemperature, m_fUnconTemperature, m_PlayerVitals.GetTemperature());
		m_wImageWidget.SetOpacity(m_fDeathProgress);
		m_wImageWidget.SetMaskProgress(m_fDeathProgress);
	}
	
	bool ShouldGetUpdated()
	{
		return m_PlayerVitals.GetTemperature()<m_fDefaultCoreTemperature;
	}
	
}
