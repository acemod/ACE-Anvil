//------------------------------------------------------------------------------------------------
//! Updates to vitals are mostly server side right now.
//! Clients can request values for vitals via ACE_Medical_NetworkComponent
//! TO DO: Inherit from ACE_Medical_BaseSystem once systems support inheritance
class ACE_Medical_MedicationSystem : ACE_Medical_BaseSystem2
{
	protected ACE_Medical_MedicationSystemSettings m_Settings;

	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_MedicationSystem)
			m_Settings = settings.m_MedicationSystem;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnFullHeal(IEntity entity)
	{
		super.OnFullHeal(entity);
		// TO DO: Get rid of all medication in body
	}
}
