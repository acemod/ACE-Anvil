//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "0", desc: "Resilience regeneration scale when second chance was triggered. The default regeneration rate will be multiplied by this factor.", params: "0 inf")]
	float m_fSecondChanceResilienceRegenScale;
	
	[Attribute(defvalue: "1", desc: "Default probability for second chance for killing blows", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fDefaultSecondChance;
	
	[Attribute(defvalue: "0", desc: "Probability for second chance for killing blows to the head", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fSecondChanceOnHead;
	
	[Attribute(defvalue: "1", desc: "Probability for second chance for killing blows to the chest", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fSecondChanceOnChest;
	
	[Attribute(defvalue: "1", desc: "Probability for second chance for killing blows to the abdomen", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fSecondChanceOnAbdomen;
	
	[Attribute(defvalue: "false", desc: "Whether second chance can be triggered when falling from great heights")]
	bool m_bSecondChanceForFallDamageEnabled;
	
	[Attribute(defvalue: "false", desc: "Whether AI can have second chance")]
	bool m_bSecondChanceForAIEnabled;
}
