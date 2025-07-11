//------------------------------------------------------------------------------------------------
class ACE_Medical_VitalStates_CharacterContext : ACE_FrameJobScheduler_IObjectContext<SCR_ChimeraCharacter>
{
	ACE_Medical_VitalStateComponent m_pComponent;
	SCR_CharacterDamageManagerComponent m_pDamageManager;
	SCR_CharacterBloodHitZone m_pBloodHitZone;
	// Reset after weaking up
	bool m_bWasInCardiacArrest = false;
	float m_fCPRSuccessCheckTimerS = 0;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_VitalStates_CharacterContext(SCR_ChimeraCharacter object)
	{
		m_pComponent = ACE_Medical_VitalStateComponent.Cast(object.FindComponent(ACE_Medical_VitalStateComponent));
		m_pDamageManager = SCR_CharacterDamageManagerComponent.Cast(object.GetDamageManager());
		m_pBloodHitZone = m_pDamageManager.GetBloodHitZone();
	}
}
