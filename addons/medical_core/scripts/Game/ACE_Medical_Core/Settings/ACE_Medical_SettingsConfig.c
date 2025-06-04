//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "0.3", desc: "Default resilience regeneration scale. The default regeneration rate will be multiplied by this factor.")]
	float m_fDefaultResilienceRegenScale;
}
