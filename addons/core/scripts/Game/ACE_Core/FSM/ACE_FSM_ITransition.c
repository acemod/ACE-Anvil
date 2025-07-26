//------------------------------------------------------------------------------------------------
//! Interface of FSM transition
//! This object should be stateless if you want to share instances between machines (e.g. shallow copies)
//! Data should be implemented in TContext instead
class ACE_FSM_ITransition<Managed TContext> : Managed
{
	protected ACE_FSM_EStateID m_eFromStateIDs;
	protected ACE_FSM_EStateID m_eToStateID;
	
	//------------------------------------------------------------------------------------------------
	//! Multiple `from` states are possible by combining ACE_FSM_EStateID bit flags
	void ACE_FSM_ITransition(ACE_FSM_EStateID fromStateIDs, ACE_FSM_EStateID toStateID)
	{
		m_eFromStateIDs = fromStateIDs;
		m_eToStateID = toStateID;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnPerform(TContext context);
	
	//------------------------------------------------------------------------------------------------
	//! Add condition for the transition here
	bool ShouldBePerformed(TContext context, float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	ACE_FSM_EStateID GetFromStateIDs()
	{
		return m_eFromStateIDs;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_FSM_EStateID GetToStateID()
	{
		return m_eToStateID;
	}
}
