//------------------------------------------------------------------------------------------------
//! Cache medical components
modded class SCR_ChimeraCharacter : ChimeraCharacter
{
	protected ACE_Medical_CardiovascularComponent m_pACE_Medical_CardiovascularComponent;
	protected ACE_Medical_MedicationComponent m_pACE_Medical_MedicationComponent;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_pACE_Medical_CardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(FindComponent(ACE_Medical_CardiovascularComponent));
		m_pACE_Medical_MedicationComponent =  ACE_Medical_MedicationComponent.Cast(FindComponent(ACE_Medical_MedicationComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_CardiovascularComponent ACE_Medical_GetCardiovascularComponent()
	{
		return m_pACE_Medical_CardiovascularComponent;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_MedicationComponent ACE_Medical_GetMedicationComponent()
	{
		return m_pACE_Medical_MedicationComponent;
	}
}
