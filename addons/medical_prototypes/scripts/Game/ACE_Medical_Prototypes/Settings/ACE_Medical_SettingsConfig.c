//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Enables second chance for shots to the torso")]
	bool m_bSecondChanceOnTorsoEnabled;
	
	[Attribute(desc: "Settings for cardiovascular system")]
	ref ACE_Medical_CardiovascularSystemSettings m_CardiovascularSystem;
	
	[Attribute(desc: "Settings for medication system")]
	ref ACE_Medical_MedicationSystemSettings m_MedicationSystem;
	
	protected static const ResourceName DEFAULT_CARDIOVASCULAR_SYSTEM_CONFIG = "{D207395D91237C8A}Configs/Systems/ACE_Medical_CardiovascularSystemSettings.conf";
	protected static const ResourceName DEFAULT_MEDICATION_SYSTEM_CONFIG = "{D60A24D9276A24DF}Configs/Systems/ACE_Medical_MedicationSystemSettings.conf";
		
	//------------------------------------------------------------------------------------------------
	//! Make sure we retain the default settings for subconfigs when left empty
	void ACE_Medical_Settings()
	{		
		if (!m_CardiovascularSystem)
			m_CardiovascularSystem = SCR_ConfigHelperT<ACE_Medical_CardiovascularSystemSettings>.GetConfigObject(DEFAULT_CARDIOVASCULAR_SYSTEM_CONFIG);
		
		if (!m_MedicationSystem)
			m_MedicationSystem = SCR_ConfigHelperT<ACE_Medical_MedicationSystemSettings>.GetConfigObject(DEFAULT_MEDICATION_SYSTEM_CONFIG);
	}
}
