//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Whether players can bleed out")]
	bool m_bBleedOutForPlayersEnabled;
}
