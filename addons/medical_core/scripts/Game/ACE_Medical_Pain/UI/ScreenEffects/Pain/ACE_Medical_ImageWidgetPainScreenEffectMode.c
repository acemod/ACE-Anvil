//------------------------------------------------------------------------------------------------
//! Class for white and grey flash effects
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_Medical_EPainEffectType, "m_eEffectType")]
class ACE_Medical_ImageWidgetPainScreenEffectMode : ACE_Medical_BasePainScreenEffectMode
{
	[Attribute(desc: "Name of the animated image widget")]
	protected string m_sImageWidgetName;
	
	[Attribute(defvalue: "1 1 1 1", desc: "Color of the effect")]
	protected ref Color m_cColor;
	
	protected ImageWidget m_wImageWidget;
	
	//------------------------------------------------------------------------------------------------
	override void InitEffect(SCR_ChimeraCharacter char, Widget root)
	{
		super.InitEffect(char, root);
		m_wImageWidget = ImageWidget.Cast(root.FindAnyWidget(m_sImageWidgetName));
		m_wImageWidget.SetColor(m_cColor);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EnableEffect(bool enable)
	{
		super.EnableEffect(enable);
		
		if (!m_wImageWidget)
			return;
		
		m_wImageWidget.SetEnabled(enable);
		m_wImageWidget.SetVisible(enable);
	}
	
	//------------------------------------------------------------------------------------------------
	override void UpdateEffect(float timeSlice)
	{
		super.UpdateEffect(timeSlice);
		float progress = m_fTimer / m_fPeriod;
		
		float opacity, maskProgress;
		if (progress < 0.5)
		{
			opacity = Math.Min(m_fMaxStrength, 2 * progress);
			maskProgress = Math.Min(m_fMaxStrength / 2, 4.5 * progress);
		}
		else
		{
			opacity = Math.Max(0, m_fMaxStrength - 0.6 * (progress - 0.5));
			maskProgress = Math.Max(0, m_fMaxStrength / 2 - 3 * (progress - 0.5));
		}
		
		m_wImageWidget.SetOpacity(opacity * m_fStrengthScale);
		m_wImageWidget.SetMaskProgress(maskProgress * m_fStrengthScale);
	}
}
