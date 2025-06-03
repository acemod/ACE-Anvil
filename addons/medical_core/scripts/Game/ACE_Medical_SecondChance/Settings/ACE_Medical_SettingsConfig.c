//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "0", desc: "Resilience regeneration scale when second chance was triggered. The default regeneration rate will be multiplied by this factor.", category: "ACE Medical")]
	float m_fSecondChanceRegenScale;
	
	[Attribute(defvalue: "0", desc: "Probability for second chance for killing blows to the head", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fSecondChanceOnHead;
	
	[Attribute(defvalue: "1", desc: "Probability for second chance for killing blows to the chest", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fSecondChanceOnChest;
	
	[Attribute(defvalue: "1", desc: "Probability for second chance for killing blows to the abdomen", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fSecondChanceOnAbdomen;
}
