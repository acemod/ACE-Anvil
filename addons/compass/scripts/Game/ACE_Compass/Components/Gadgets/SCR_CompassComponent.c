//------------------------------------------------------------------------------------------------
modded class SCR_CompassComponent : SCR_GadgetComponent {
    protected ACE_Compass_Display m_ACE_Compass_Display;

    //------------------------------------------------------------------------------------------------
    override void ActivateGadgetUpdate() {
        super.ActivateGadgetUpdate();

        if (m_CharacterOwner != SCR_PlayerController.GetLocalControlledEntity())
            return;

        // https://discord.com/channels/105462288051380224/976155351999201390/1138453903562768414
        if (!m_ACE_Compass_Display)
            m_ACE_Compass_Display = ACE_Compass_Display.Cast(SCR_HUDManagerComponent.GetHUDManager().FindInfoDisplay(ACE_Compass_Display));
    }

    //------------------------------------------------------------------------------------------------
    override void Update(float timeSlice) {
        super.Update(timeSlice);

        // Only run on owner
        if (m_CharacterOwner != SCR_PlayerController.GetLocalControlledEntity())
            return;

        bool displayShown = m_ACE_Compass_Display.IsShown();

        if (m_bIsInMapMode || !m_bFocused) {
            if (displayShown)
                m_ACE_Compass_Display.Show(false);

            return;
        }

        if (!displayShown) {
            m_ACE_Compass_Display.SetCompassEntity(GetOwner());
            m_ACE_Compass_Display.Show(true, UIConstants.FADE_RATE_SLOW);
        }

        m_ACE_Compass_Display.UpdateBearing(m_fNeedleAngle);
    }

    //------------------------------------------------------------------------------------------------
    //! Handles the cases when a player falls unconscious while having the compass focused
    override void DeactivateGadgetUpdate() {
        super.DeactivateGadgetUpdate();

        if (m_CharacterOwner != SCR_PlayerController.GetLocalControlledEntity())
            return;

        if (m_ACE_Compass_Display && m_ACE_Compass_Display.IsShown())
            m_ACE_Compass_Display.Show(false);
    }
}
