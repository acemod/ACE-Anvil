
modded class SCR_CompassComponent : SCR_GadgetComponentClass
{
	protected ACE_Compass m_InfoDisplay;
	
	override void Update(float timeSlice)
	{
		super.Update(timeSlice);
				
		if (m_bIsInMapMode || !m_bFocused)
		{
			GetHud().Show(false);
			return;
		}
		
		if (m_CharacterOwner != SCR_PlayerController.GetLocalControlledEntity())
			return;
		
		GetHud().Show(true);
	}
	
	protected ACE_Compass GetHud()
	{
		if (!m_InfoDisplay)
			m_InfoDisplay = ACE_Compass.Cast(SCR_HUDManagerComponent.GetHUDManager().FindInfoDisplay(ACE_Compass));
		
		return m_InfoDisplay;
	}
}