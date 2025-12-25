//------------------------------------------------------------------------------------------------
modded class ACE_Medical_CharacterContext : ACE_FrameJobScheduler_IObjectContext<SCR_ChimeraCharacter>
{
	float m_fVentFluxO2;
	float m_fVentFluxCO2;
	float m_fPerfFluxO2;
	float m_fPerfFluxCO2;
	float m_fOxygenConsumption;
	float m_fCO2Production;
	float m_fNextVomitUpdateTime;
}
