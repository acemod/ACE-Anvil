//------------------------------------------------------------------------------------------------
class ACE_Medical_VitalStateMachine : ACE_FSM_Machine<ACE_Medical_CharacterContext>
{
	protected ACE_Medical_EVitalStateID m_eCurrentStateID;
	
	//------------------------------------------------------------------------------------------------
	//! Check for forced transitions on the component
	override void OnUpdate(float timeSlice)
	{
		ACE_Medical_EVitalStateID stateID = m_pContext.m_pVitals.GetVitalStateID();
		if (m_iCurrentStateIdx >= 0 && stateID != m_eCurrentStateID)
			ChangeState(stateID);
		
		super.OnUpdate(timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update state ID on the component
	override protected void ChangeStateByIdx(int newStateIdx)
	{
		if (newStateIdx >= 0)
		{
			m_eCurrentStateID = m_aStates[newStateIdx].GetID();
			m_pContext.m_pVitals.SetVitalStateID(m_eCurrentStateID);
		}
		else
		{
			m_eCurrentStateID = ACE_Medical_EVitalStateID.STOP;
		}
		
		super.ChangeStateByIdx(newStateIdx);
	}
}
