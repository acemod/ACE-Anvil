//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot:true)]
class ACE_Medical_AEDSystemSettings
{
	[Attribute(defvalue: "false", desc: "AEDs can have individual charge times. Select to override globally.")]
	bool m_bAEDChargeTimeOverride;
	
	[Attribute(defvalue: "5", desc: "Charge Time Override [s]")]
	int m_iAEDChargeTimeOverrideValue;
	
	[Attribute(defvalue: "false", desc: "AEDs can have individual analysis times. Select to override globally.")]
	bool m_bAEDAnalysisTimeOverride;
	
	[Attribute(defvalue: "5", desc: "Analysis Time Override [s]")]
	int m_iAEDAnalysisTimeOverrideValue;
	
	void ACE_Medical_AEDSystemSettings()
	{
	}
}