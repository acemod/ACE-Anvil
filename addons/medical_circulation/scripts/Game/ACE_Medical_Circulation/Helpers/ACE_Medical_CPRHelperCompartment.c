//------------------------------------------------------------------------------------------------
class ACE_Medical_CPRHelperCompartmentClass : ACE_AnimationHelperCompartmentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_CPRHelperCompartment : ACE_AnimationHelperCompartment
{
	protected ACE_Medical_VitalsComponent m_pPatientVitals;
	
	//------------------------------------------------------------------------------------------------
	void SetPatient(notnull SCR_ChimeraCharacter patient)
	{
		m_pPatientVitals = ACE_Medical_VitalsComponent.Cast(patient.FindComponent(ACE_Medical_VitalsComponent));
		if (m_pPatientVitals)
			m_pPatientVitals.SetIsCPRPerformed(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnCompartmentLeft()
	{
		super.OnCompartmentLeft();
		
		if (m_pPatientVitals)
			m_pPatientVitals.SetIsCPRPerformed(false);
	}
}
