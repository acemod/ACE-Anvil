//------------------------------------------------------------------------------------------------
//! Interface of FSM state
//! This object should be stateless if you want to share instances between machines (e.g. shallow copies)
//! Data should be implemented in TContext instead
class ACE_FSM_IState<Managed TContext> : Managed
{
	protected ACE_FSM_EStateID m_eID;
	
	//------------------------------------------------------------------------------------------------
	void ACE_FSM_IState(ACE_FSM_EStateID id)
	{
		m_eID = id;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnUpdate(TContext context, float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	void OnEnter(TContext context);
	
	//------------------------------------------------------------------------------------------------
	void OnExit(TContext context);
	
	//------------------------------------------------------------------------------------------------
	ACE_FSM_EStateID GetID()
	{
		return m_eID;
	}
}
