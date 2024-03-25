//------------------------------------------------------------------------------------------------
modded class SCR_CompassComponent : SCR_GadgetComponent
{	
	protected IEntity m_pACE_Compass_Entity;
	protected ACE_Compass_Display m_ACE_Compass_Display;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_pACE_Compass_Entity = owner;
	}

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
		bool isDisplayShown = m_ACE_Compass_Display.IsShown();

		if (m_bIsInMapMode || !m_bFocused)
		{
			if (isDisplayShown)
			{
				m_ACE_Compass_Display.Show(false);
			};

			return;
		}

		if (m_CharacterOwner != SCR_PlayerController.GetLocalControlledEntity())
			return;

		// https://discord.com/channels/105462288051380224/976155351999201390/1138453903562768414
		if (!isDisplayShown)
		{
			m_ACE_Compass_Display.SetCompassEntity(m_pACE_Compass_Entity);
			m_ACE_Compass_Display.Show(true, UIConstants.FADE_RATE_SLOW);
		}
		else
		{
			m_ACE_Compass_Display.UpdateBearing(m_fNeedleAngle);
		}
	}
}
