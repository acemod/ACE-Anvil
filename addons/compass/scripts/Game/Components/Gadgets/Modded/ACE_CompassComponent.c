
modded class SCR_CompassComponent : SCR_GadgetComponentClass
{	
	protected IEntity m_CompassEntity;
	protected ACE_Compass m_InfoDisplay;
	protected bool m_bShown = false;
	
	override void EOnInit(IEntity owner)
	{
		m_CompassEntity = owner;
	}

	override void Update(float timeSlice)
	{
		super.Update(timeSlice);

		if (m_bIsInMapMode || !m_bFocused)
		{
			if (m_bShown)
			{
				m_bShown = false;
				GetHud().Show(false);
			};

			return;
		}

		if (m_CharacterOwner != SCR_PlayerController.GetLocalControlledEntity())
		{
			return;
		}

		// https://discord.com/channels/105462288051380224/976155351999201390/1138453903562768414
		if (!m_bShown)
		{
			m_bShown = true;
			GetHud().SetCompassEntity(m_CompassEntity);
			GetHud().Show(true, UIConstants.FADE_RATE_SLOW);
		}
		else
		{
			GetHud().UpdateBearing(m_fNeedleAngle);
		}
	}

	protected ACE_Compass GetHud()
	{
		if (!m_InfoDisplay)
			m_InfoDisplay = ACE_Compass.Cast(SCR_HUDManagerComponent.GetHUDManager().FindInfoDisplay(ACE_Compass));

		return m_InfoDisplay;
	}
}
