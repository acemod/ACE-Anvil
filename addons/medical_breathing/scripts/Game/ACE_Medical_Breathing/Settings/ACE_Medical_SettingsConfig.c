//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(desc: "Settings for respiratory system")]
	ref ACE_Medical_RespiratorySystemSettings m_RespiratorySystem;
	
	protected static const ResourceName DEFAULT_RESPIRATORY_SYSTEM_CONFIG = "{D207395D91237C8A}Configs/Systems/ACE_Medical_CardiovascularSystemSettings.conf";
		
	//------------------------------------------------------------------------------------------------
	//! Make sure we retain the default settings for subconfigs when left empty
	void ACE_Medical_Settings()
	{		
		if (!m_RespiratorySystem)
			m_RespiratorySystem = SCR_ConfigHelperT<ACE_Medical_RespiratorySystemSettings>.GetConfigObject(DEFAULT_RESPIRATORY_SYSTEM_CONFIG);
	}
}
