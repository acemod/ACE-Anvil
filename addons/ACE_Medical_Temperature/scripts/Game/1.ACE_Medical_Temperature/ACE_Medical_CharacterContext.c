//------------------------------------------------------------------------------------------------
modded class ACE_Medical_CharacterContext : ACE_FrameJobScheduler_IObjectContext<SCR_ChimeraCharacter>
{
	SignalsManagerComponent m_pSignalsManager;
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_CharacterContext(SCR_ChimeraCharacter object)
	{
		if (!m_pSignalsManager){
			m_pSignalsManager=SignalsManagerComponent.Cast(object.FindComponent(SignalsManagerComponent));
		}
		
	}
	
	//------------------------------------------------------------------------------------------------

}
