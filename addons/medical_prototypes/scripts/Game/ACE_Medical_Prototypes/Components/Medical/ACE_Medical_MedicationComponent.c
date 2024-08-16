//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationComponentClass : ACE_Medical_BaseComponent2Class
{
}

//------------------------------------------------------------------------------------------------
//! Updates to vitals are mostly server side right now
//! Clients can request values for vitals via ACE_Medical_NetworkComponent
//! TO DO: Inherit from ACE_Medical_BaseComponent once systems support inheritance
class ACE_Medical_MedicationComponent : ACE_Medical_BaseComponent2
{	
	//------------------------------------------------------------------------------------------------
	//! Register this component at ACE_Medical_CardiovascularSystem
	override typename GetAssociatedSystemType()
	{
		return ACE_Medical_MedicationSystem;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();	
	}
}
