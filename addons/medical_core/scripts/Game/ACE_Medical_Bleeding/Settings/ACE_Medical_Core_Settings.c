//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Core_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Whether players can bleed out")]
	bool m_bBleedOutForPlayersEnabled;
	
	[Attribute(defvalue: "1", desc: "Character bleeding rate multiplier", params: "0 inf")]
	float m_fBleedingRateScale;
	
	[Attribute(defvalue: "-1", desc: "Maximum possibe bleeding rate in ml/s (Negative value means unlimited)", params: "-1 inf")]
	float m_fMaxTotalBleedingRate;
	
	[Attribute(defvalue: "1", desc: "Character blood regeneration rate multiplier", params: "0 inf")]
	float m_fBloodRegenScale;
}
