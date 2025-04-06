//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
modded class ACE_Medical_Settings
{
	[Attribute(desc: "Settings for defibrillation system")]
	ref ACE_Medical_DefibrillationSystemSettings m_DefibrillationSystem;

	protected static const ResourceName DEFAULT_DEFIBRILLATION_SYSTEM_CONFIG = "{F60BFB17A2E14FA9}Configs/Systems/ACE_Medical_DefibrillationSystemSettings.conf";
		
	//------------------------------------------------------------------------------------------------
	//! Make sure we retain the default settings for subconfigs when left empty
	void ACE_Medical_Settings()
	{
		super.this;
		if (!m_DefibrillationSystem)
			m_DefibrillationSystem = SCR_ConfigHelperT<ACE_Medical_DefibrillationSystemSettings>.GetConfigObject(DEFAULT_DEFIBRILLATION_SYSTEM_CONFIG);
	}
}
