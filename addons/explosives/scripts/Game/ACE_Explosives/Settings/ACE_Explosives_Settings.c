//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
class ACE_Explosives_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Metal/Mine detectors will detect any metal object instead of just mines when enabled.")]
	bool m_bAnyMetalDetectionEnabled;
	
	[Attribute(defvalue: "true", desc: "Dead man's switch only triggers while in hand if true. Otherwise it can trigger from anywhere in the inventory.")]
	bool m_bDeadManSwitchOnlyActiveInHand;
}
