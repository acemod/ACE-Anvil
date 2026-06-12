//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Compass_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Enables simulation of magnatic north and makes compass point to it.")]
	bool m_bMagneticDeclinationEnabled;
}
