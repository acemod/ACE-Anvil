//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Core_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "10", desc: "Amount of health that is restored each execution of the medical kit outside of medical facilities.", uiwidget: UIWidgets.Slider, params: "0 inf")]
	float m_fMedicalKitHealingPerExecution;
}
