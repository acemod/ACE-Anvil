//------------------------------------------------------------------------------------------------
modded class ACE_Medical_CharacterContext : ACE_FrameJobScheduler_IObjectContext<SCR_ChimeraCharacter>
{
	float m_fVentFluxO2; // mmol/s
	float m_fVentFluxCO2; // mmol/s
	float m_fPerfFluxO2; // mmol/s
	float m_fPerfFluxCO2; // mmol/s
	float m_fOxygenConsumption; // mmol/s
	float m_fCO2Production; // mmol/s
	float m_fNextVomitUpdateTime;
}
