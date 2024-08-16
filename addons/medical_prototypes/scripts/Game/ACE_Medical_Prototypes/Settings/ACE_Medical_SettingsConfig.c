//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Enables second chance for shots to the chest")]
	bool m_bSecondChanceEnabled;
	
	[Attribute(desc: "Settings for medication system")]
	ref ACE_Medical_MedicationSystemSettings m_MedicationSystem;
	
	[Attribute(desc: "Settings for cardiovascular system")]
	ref ACE_Medical_CardiovascularSystemSettings m_CardiovascularSystem;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_Settings()
	{
		if (!m_CardiovascularSystem)
			m_CardiovascularSystem = new ACE_Medical_CardiovascularSystemSettings();
	}
}
