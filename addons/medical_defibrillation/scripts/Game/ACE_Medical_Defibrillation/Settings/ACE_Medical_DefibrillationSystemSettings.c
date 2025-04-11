//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_Medical_DefibrillationSystemSettings
{
	[Attribute(defvalue: "false", desc: "AEDs can have individual charge times. Select to override globally.")]
	bool m_bAEDChargeTimeOverride;
	
	[Attribute(defvalue: "5.0", desc: "Charge Time Override [s]", params: "1.0 5.0 0.1", precision: 1)]
	float m_fAEDChargeTimeOverrideValue;
	
	[Attribute(defvalue: "false", desc: "AEDs can have individual analysis times. Select to override globally.")]
	bool m_bAEDAnalysisTimeOverride;
	
	[Attribute(defvalue: "5.0", desc: "Analysis Time Override [s]", params: "1.0, 5.0, 0.1", precision: 1)]
	float m_fAEDAnalysisTimeOverrideValue;
	
	void ACE_Medical_DefibrillationSystemSettings()
	{
	}
}