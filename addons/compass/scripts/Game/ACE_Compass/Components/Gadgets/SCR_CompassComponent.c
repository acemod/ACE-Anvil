//------------------------------------------------------------------------------------------------
modded class SCR_CompassComponent : SCR_GadgetComponent
{
	protected ACE_Compass_Display m_ACE_Compass_Display;
	
	//------------------------------------------------------------------------------------------------
	override void ActivateGadgetUpdate()
	{
		super.ActivateGadgetUpdate();
		
		if (!m_ACE_Compass_Display)
			m_ACE_Compass_Display = ACE_Compass_Display.Cast(SCR_HUDManagerComponent.GetHUDManager().FindInfoDisplay(ACE_Compass_Display));
	}
	
	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{
		super.Update(timeSlice);
				
		if (m_bIsInMapMode || !m_bFocused)
		{
			m_ACE_Compass_Display.Show(false);
			return;
		}
		
		if (m_CharacterOwner != SCR_PlayerController.GetLocalControlledEntity())
			return;
		
		m_ACE_Compass_Display.Show(true);
	}
}