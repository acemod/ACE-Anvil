//------------------------------------------------------------------------------------------------
//! Class for chromatic aberration and radial blur effects 
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_Medical_EPainEffectType, "m_eEffectType")]
class ACE_Medical_PostProcessPainScreenEffectMode : ACE_Medical_BasePainScreenEffectMode
{
	[Attribute(uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(PostProcessEffectType))]
	protected PostProcessEffectType m_ePPType;
	
	[Attribute(params: "emat")]
	protected ResourceName m_sMaterialName;
	
	protected ref Material m_EffectMaterial;
	protected static const int POST_PROCESS_EFFECT_PRIORITY = 7;
	protected static const float POWER_FADEIN_SPEED = 2;
	protected static const float POWER_FADEOUT_SPEED = 6;
	
	//------------------------------------------------------------------------------------------------
	override void InitEffect(SCR_ChimeraCharacter char, Widget root)
	{
		super.InitEffect(char, root);
		char.GetWorld().SetCameraPostProcessEffect(char.GetWorld().GetCurrentCameraId(), POST_PROCESS_EFFECT_PRIORITY, m_ePPType, m_sMaterialName);
		m_EffectMaterial = Material.GetMaterial(m_sMaterialName);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EnableEffect(bool enable)
	{
		super.EnableEffect(enable);
		
		if (m_EffectMaterial)
			m_EffectMaterial.SetParam("Enabled", enable);
	}
	
	//------------------------------------------------------------------------------------------------
	override void UpdateEffect(float timeSlice)
	{
		super.UpdateEffect(timeSlice);
		float progress = m_fTimer / m_fPeriod;
		
		float strength;
		if (progress < 0.5)
			strength = Math.Min(m_fMaxStrength, POWER_FADEIN_SPEED * progress);
		else
			strength = Math.Max(0, m_fMaxStrength - POWER_FADEOUT_SPEED * (progress - 0.5));
		
		strength *=  m_fStrengthScale;
		m_EffectMaterial.SetParam("PowerX", strength);
		m_EffectMaterial.SetParam("PowerY", strength);
	}
}
