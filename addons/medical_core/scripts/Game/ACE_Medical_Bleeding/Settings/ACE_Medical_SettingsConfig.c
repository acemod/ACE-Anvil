//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Whether players can bleed out")]
	bool m_bBleedOutForPlayersEnabled;
	
	[Attribute(defvalue: "1", desc: "Character bleeding rate multiplier")]
	float m_fBleedRateScale;
	
	[Attribute(defvalue: "-1", desc: "Maximum possibe bleeding rate in ml/s (Negative value means unlimited)")]
	float m_fMaxTotalBleedingRate;
	
	[Attribute(defvalue: "1", desc: "Character blood regeneration rate multiplier")]
	float m_fBloodRegenScale;
}
