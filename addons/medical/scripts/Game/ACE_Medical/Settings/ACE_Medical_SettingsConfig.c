//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "0", desc: "Resilience regeneration scale when second chance was triggered. The default regeneration rate will be multiplied by this factor.", category: "ACE Medical")]
	float m_fSecondChanceRegenScale;
	
	[Attribute(defvalue: "false", desc: "Enables second chance on headshots")]
	bool m_bSecondChanceOnHeadEnabled;
	
	[Attribute(defvalue: "1", desc: "Healing consumes supplies when enabled. Ignored when global supply usage is disabled.")]
	bool m_bHealSupplyUsageEnabled;
	
	[Attribute(defvalue: "0.5", desc: "Maximum scaled health (from 0 to 1) that a medical kit can heal to outside of medical facilities")]
	float m_fMedicalKitMaxHealScaled;
}
