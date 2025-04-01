//------------------------------------------------------------------------------------------------
//! Cache medical components
modded class SCR_ChimeraCharacter : ChimeraCharacter
{
	protected ACE_Medical_RespiratoryComponent m_pACE_Medical_RespiratoryComponent;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_pACE_Medical_RespiratoryComponent = ACE_Medical_RespiratoryComponent.Cast(FindComponent(ACE_Medical_RespiratoryComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_RespiratoryComponent ACE_Medical_GetRespiratoryComponent()
	{
		return m_pACE_Medical_RespiratoryComponent;
	}
}
