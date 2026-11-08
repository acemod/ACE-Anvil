//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
class ACE_Explosives_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Metal/Mine detectors will detect any metal object instead of just mines when enabled.")]
	bool m_bAnyMetalDetectionEnabled;
}
