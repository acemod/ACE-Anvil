//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Whether organ hit zones should be enabled")]
	bool m_bOrganHitZonesEnabled;
}
