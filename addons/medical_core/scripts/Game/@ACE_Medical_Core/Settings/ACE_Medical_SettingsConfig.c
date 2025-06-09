//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "0.3", desc: "Default resilience regeneration scale. The default regeneration rate will be multiplied by this factor.")]
	float m_fDefaultResilienceRegenScale;
	
	[Attribute(defvalue: "0.33", desc: "Can only apply epinephrine when patient has at least this health (ignored in advanced medication).", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fMinHealthScaledForEpinephrine;
}
