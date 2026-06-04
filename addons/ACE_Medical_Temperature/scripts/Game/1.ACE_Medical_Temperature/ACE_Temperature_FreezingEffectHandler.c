class ACE_Temperature_FreezingEffectHandler :SCR_BaseScreenEffect
{
	protected ACE_Medical_VitalsComponent m_PlayerVitals;
	protected float m_fDefaultCoreTemperature;
	protected float m_fUnconTemperature;
	
	protected float m_fYTemperature;
	protected float m_fXTemperature;
	protected float m_fInterpProgress;
	
	
	ImageWidget m_wImageWidget;
	
	override void DisplayControlledEntityChanged(IEntity from, IEntity to){
		ClearEffects();

		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(to);
		if (!char)
			return;
		
		m_PlayerVitals=ACE_Medical_VitalsComponent.Cast(char.FindComponent(ACE_Medical_VitalsComponent));
		m_wImageWidget=ImageWidget.Cast(m_wRoot.FindAnyWidget("ACE_Temperature_Freezing"));
		m_fDefaultCoreTemperature=ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings().m_fDefaultCoreTemperature;
		m_fUnconTemperature=ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings().m_CriticalThresholds.m_fTemperatureThreshold;
		m_fYTemperature=m_fDefaultCoreTemperature;
		m_fXTemperature=m_fDefaultCoreTemperature;
	}
	
	void EnableEffect(bool enable)
	{
		m_wImageWidget.SetEnabled(enable);
		m_wImageWidget.SetVisible(enable);
	}
	
	override void UpdateEffect(float timeSlice)
	{
		if (m_PlayerVitals.GetTemperature()!=m_fXTemperature){
			m_fInterpProgress=0;
			m_fYTemperature=m_fXTemperature;
			m_fXTemperature=m_PlayerVitals.GetTemperature();
		}
		float m_fCurrentTemperature = Math.Lerp(m_fYTemperature,m_fXTemperature,m_fInterpProgress);
		m_fInterpProgress+=timeSlice;
		float m_fDeathProgress=Math.InverseLerp(m_fDefaultCoreTemperature-1, m_fUnconTemperature, m_fCurrentTemperature);
		
		m_wImageWidget.SetOpacity(m_fDeathProgress);
		m_wImageWidget.SetMaskProgress(Math.Min(m_fDeathProgress/5,0.12));
	}
	
	bool ShouldGetUpdated()
	{
		return m_PlayerVitals.GetTemperature()<m_fDefaultCoreTemperature;
	}
	
}
