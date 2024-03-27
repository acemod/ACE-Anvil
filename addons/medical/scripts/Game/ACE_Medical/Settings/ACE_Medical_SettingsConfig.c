//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "1", desc: "Enables second chance on headshots")]
	float m_bSecondChanceOnHeadEnabled;
}
