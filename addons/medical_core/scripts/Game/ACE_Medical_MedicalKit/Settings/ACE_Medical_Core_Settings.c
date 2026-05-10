//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Core_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "1", desc: "Maximum scaled health (from 0 to 1) that a medical kit can heal to outside of medical facilities.", uiwidget: UIWidgets.Slider, params: "0 1")]
	float m_fMedicalKitMaxHealScaled;
	
	[Attribute(defvalue: "10", desc: "Amount of health that is restored each execution of the medical kit outside of medical facilities.", uiwidget: UIWidgets.Slider, params: "0 inf")]
	float m_fMedicalKitHealingPerExecution;
}
