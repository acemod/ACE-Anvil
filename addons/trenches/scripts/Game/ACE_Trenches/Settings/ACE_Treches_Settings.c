//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
class ACE_Treches_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "2", desc: "How much more time it takes to build structures placed with the E-tool.")]
	float m_fBuildTimeScale;
}
