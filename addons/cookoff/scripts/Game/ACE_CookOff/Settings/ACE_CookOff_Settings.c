//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_CookOff_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "5.0", desc: "Minimum time required for ammo cook-off to start after vehicle caught fire [s]", params: "0 inf")]
	float m_fMinCookOffDelay;
	
	[Attribute(defvalue: "11.0", desc: "Maximum time required for ammo cook-off to start after vehicle caught fire [s]", params: "0 inf")]
	float m_fMaxCookOffDelay;
}
