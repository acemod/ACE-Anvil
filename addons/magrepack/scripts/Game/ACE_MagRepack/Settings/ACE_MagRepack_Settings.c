//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
class ACE_MagRepack_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "1.0", desc: "Scale for time it takes to transfer a round between magazines.", params: "0 inf")]
	float m_fPackingTimeScale;
	
	[Attribute(defvalue: "0.1", desc: "Scale for time it takes to transfer a round between ammunition belts.", params: "0 inf")]
	float m_fBeltPackingTimeScale;
}
