//------------------------------------------------------------------------------------------------
modded class ACE_Medical_CharacterContext : ACE_FrameJobScheduler_IObjectContext<SCR_ChimeraCharacter>
{
	SignalsManagerComponent m_signalsManager;
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_CharacterContext(SCR_ChimeraCharacter object)
	{
		if (!m_signalsManager){
			m_signalsManager=SignalsManagerComponent.Cast(object.FindComponent(SignalsManagerComponent));
		}
		
	}
	
	//------------------------------------------------------------------------------------------------

}
