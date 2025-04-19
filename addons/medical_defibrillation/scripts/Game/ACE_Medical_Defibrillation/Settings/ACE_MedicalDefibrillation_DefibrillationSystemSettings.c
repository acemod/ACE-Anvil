//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_MedicalDefibrillation_DefibrillationSystemSettings
{
	[Attribute(defvalue: "false", desc: "Defibrillators can have individual charge times. Select to override globally.")]
	bool m_bDefibrillatorChargeTimeOverride;
	
	[Attribute(defvalue: "false", desc: "AEDs can have individual analysis times. Select to override globally.")]
	bool m_bAEDAnalysisTimeOverride;
	
	void ACE_MedicalDefibrillation_DefibrillationSystemSettings()
	{
	}
}