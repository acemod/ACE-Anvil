//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_Medical_EPainEffectType, "m_eEffectType")]
class ACE_Medical_BasePainScreenEffectMode
{
	[Attribute(uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ACE_Medical_EPainEffectType))]
	protected ACE_Medical_EPainEffectType m_eEffectType;

	[Attribute(defvalue: "1")]
	protected float m_fStrengthScale;
	
	[Attribute(defvalue: "0.5", desc: "Frequency of the effect")]
	protected float m_fFrequency;
	
	protected bool m_bEnabled;
	protected float m_fTimer = 0;
	protected float m_fPeriod;
	protected bool m_bIsTerminating;
	protected float m_fMaxStrength;
	protected SCR_CharacterDamageManagerComponent m_pDamageManager;
	
	void InitEffect(SCR_ChimeraCharacter char, Widget root)
	{
		m_fPeriod = 1 / m_fFrequency;
		m_pDamageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
	}
	
	//------------------------------------------------------------------------------------------------
	void EnableEffect(bool enable)
	{
		// Cancel terminations
		if (m_bIsTerminating)
			m_bIsTerminating = false;
		
		if (m_bEnabled == enable)
			return;
		
		m_bEnabled = enable;
		
		if (!enable)
			return;
		
		m_fTimer = 0;
		m_fMaxStrength = m_pDamageManager.ACE_Medical_GetPainIntensity();
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateEffect(float timeSlice)
	{
		m_fTimer += timeSlice;
		if (m_fTimer <= m_fPeriod)
			return;
		
		m_fTimer = 0;
		m_fMaxStrength = m_pDamageManager.ACE_Medical_GetPainIntensity();
		
		if (m_bIsTerminating)
			EnableEffect(false);
	}
	
	//------------------------------------------------------------------------------------------------
	void TerminateEffect()
	{
		m_bIsTerminating = true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ShouldGetUpdated()
	{
		return m_bEnabled || m_bIsTerminating;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_EPainEffectType GetEffectType()
	{
		return m_eEffectType;
	}
}
