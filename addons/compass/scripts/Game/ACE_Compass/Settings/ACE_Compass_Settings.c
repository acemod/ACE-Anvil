//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Compass_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Whether compass should point to magnetic north instead of true north.")]
	bool m_bMagneticDeclinationEnabled;
}
