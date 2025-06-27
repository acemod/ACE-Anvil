//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
modded class ACE_Medical_Settings
{
	[Attribute(desc: "Settings for defibrillation system")]
	ref ACE_MedicalDefibrillation_DefibrillationSystemSettings m_DefibrillationSystem;

	protected static const ResourceName DEFAULT_DEFIBRILLATION_SYSTEM_CONFIG = "{B420F77B4042645D}Configs/Systems/ACE_MedicalDefibrillation_DefibrillationSystemSettings.conf";
		
	//------------------------------------------------------------------------------------------------
	//! Make sure we retain the default settings for subconfigs when left empty
	void ACE_Medical_Settings()
	{
		super.this;
		if (!m_DefibrillationSystem)
			m_DefibrillationSystem = SCR_ConfigHelperT<ACE_MedicalDefibrillation_DefibrillationSystemSettings>.GetConfigObject(DEFAULT_DEFIBRILLATION_SYSTEM_CONFIG);
	}
}
