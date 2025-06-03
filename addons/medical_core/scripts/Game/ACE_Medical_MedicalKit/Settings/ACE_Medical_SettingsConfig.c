//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Healing consumes supplies when enabled. Ignored when global supply usage is disabled.")]
	bool m_bHealSupplyUsageEnabled;
	
	[Attribute(defvalue: "0.5", desc: "Maximum scaled health (from 0 to 1) that a medical kit can heal to outside of medical facilities", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fMedicalKitMaxHealScaled;
}
