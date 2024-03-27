//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "false", desc: "Enables second chance on headshots")]
	bool m_bSecondChanceOnHeadEnabled;
}
