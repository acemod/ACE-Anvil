//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Whether pain is enabled")]
	bool m_bPainEnabled;
}
