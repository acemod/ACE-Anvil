//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_DefibStateMachine : ACE_FSM_Machine<ACE_Medical_Defibrillation_DefibContext>
{
	protected ACE_Medical_Defibrillation_EDefibStateID m_eCurrentStateID;
	
	//------------------------------------------------------------------------------------------------
	//! Check for forced transitions on the component
	override void OnUpdate(float timeSlice)
	{
		ACE_Medical_Defibrillation_EDefibStateID stateID = m_pContext.m_pDefibrillator.GetDefibStateID();
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
			m_pContext.m_pDefibrillator.SetDefibStateID(m_eCurrentStateID);
		}
		else
		{
			m_eCurrentStateID = ACE_Medical_Defibrillation_EDefibStateID.STOP;
		}
		
		super.ChangeStateByIdx(newStateIdx);
	}
}