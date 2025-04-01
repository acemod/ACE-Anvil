//------------------------------------------------------------------------------------------------
//! Cache medical components
modded class SCR_ChimeraCharacter : ChimeraCharacter
{
	protected ACE_Medical_RespiratoryComponent m_pACE_Medical_RespiratoryComponent;
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_RespiratoryComponent ACE_Medical_GetRespiratoryComponent()
	{
		if (!m_pACE_Medical_RespiratoryComponent)
			m_pACE_Medical_RespiratoryComponent = ACE_Medical_RespiratoryComponent.Cast(FindComponent(ACE_Medical_RespiratoryComponent));
		
		return m_pACE_Medical_RespiratoryComponent;
	}
}
