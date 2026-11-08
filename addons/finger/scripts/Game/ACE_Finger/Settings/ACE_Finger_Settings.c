//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
class ACE_Finger_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "1000", desc: "Maximum pointing distance in meters.", params: "0 inf")]
	float m_fMaxPointingDistanceM;
	
	[Attribute(defvalue: "true", desc: "Whether the ping can attach to entities.")]
	bool m_bCanPingAttach;
	
	[Attribute(defvalue: "10", desc: "Range of the ping in meters. Only players in range will see it. Anyone can see it if negative.", params: "-1 inf")]
	float m_fPingRangeM;
	
	[Attribute(defvalue: "10", desc: "Range of map pointing in meters. Only players in range will see it. Anyone can see it if negative.", params: "-1 inf")]
	float m_fMapPointingRangeM;
}

