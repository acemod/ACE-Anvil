//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Whether organ hit zones should be enabled")]
	bool m_bOrganHitZonesEnabled;
	
	[Attribute(defvalue: "0", desc: "Probability for second chance for killing blows to the heart", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fSecondChanceOnHeart;
}
