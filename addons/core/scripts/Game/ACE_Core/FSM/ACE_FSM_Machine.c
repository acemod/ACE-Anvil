//------------------------------------------------------------------------------------------------
//! Implementation of a finite state machine
//! Data to be passed to states should be defined in TContext
class ACE_FSM_Machine<Managed TContext> : ACE_IFrameJob
{
	// Unique data
	protected ref TContext m_pContext = null;
	protected int m_iCurrentStateIdx = -1;
	
	// Shared data (Can be shared between instances through ACE_FSM_Machine::CopyFrom)
	protected ref array<ref ACE_FSM_IState<TContext>> m_aStates = {};
	protected ref array<ref ACE_FSM_ITransition<TContext>> m_aTransitions = {};
	protected ref array<ref array<ACE_FSM_ITransition<TContext>>> m_aTransitionTable;
	
	//------------------------------------------------------------------------------------------------
	//! Adds a state
	//! If a state with the same ID already exists, it will replace it
	void AddState(ACE_FSM_IState<TContext> newState)
	{
		ACE_FSM_EStateID newID = newState.GetID();
		
		foreach (int i, ACE_FSM_IState<TContext> state : m_aStates)
		{
			if (state.GetID() == newID)
			{
				m_aStates[i] = newState;
				return;
			}
		}
		
		m_aStates.Insert(newState);
		
		if (IsRunning())
			UpdateTransitionTable();
		else
			m_aTransitionTable = null;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Adds a transition
	//! If a transition between the same states already exists, it will replace it
	void AddTransition(ACE_FSM_ITransition<TContext> newTransition)
	{
		ACE_FSM_EStateID newToIDs = newTransition.GetToStateID();
		ACE_FSM_EStateID newFromID = newTransition.GetFromStateIDs();
		
		foreach (int i, ACE_FSM_ITransition<TContext> transition : m_aTransitions)
		{
			if ((transition.GetToStateID() == newToIDs) && (transition.GetFromStateIDs() == newFromID))
			{
				m_aTransitions[i] = newTransition;
				return;
			}
		}
		
		m_aTransitions.Insert(newTransition);
		
		if (IsRunning())
			UpdateTransitionTable();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates current state and handles transitions
	//! Reason for the order:
	//! As the current state's OnUpdate often integrates context variables over timeSlice, the
	//! evaulation of transition conditions should be done afterwards
	override void OnUpdate(float timeSlice)
	{
		super.OnUpdate();
		m_aStates[m_iCurrentStateIdx].OnUpdate(m_pContext, timeSlice);
		HandleTransitions(timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Initializes elements and transition table
	override void OnStart()
	{
		super.OnStart();
		
		if (!m_aTransitionTable)
			UpdateTransitionTable();
		
		ChangeStateByIdx(0);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clean up current state
	override void OnStop()
	{
		super.OnStop();
		ChangeStateByIdx(-1);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateTransitionTable()
	{
		m_aTransitionTable = {};
		m_aTransitionTable.Reserve(m_aStates.Count());
		
		foreach (ACE_FSM_IState<TContext> state : m_aStates)
		{
			ACE_FSM_EStateID id = state.GetID();
			array<ACE_FSM_ITransition<TContext>> availableTransitions = {};
			
			foreach (ACE_FSM_ITransition<TContext> transition : m_aTransitions)
			{
				if (transition.GetFromStateIDs() & id)
					availableTransitions.Insert(transition);
			}
			
			m_aTransitionTable.Insert(availableTransitions);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Checks transition conditions and performs the first eligible transition
	protected void HandleTransitions(float timeSlice)
	{
		foreach (ACE_FSM_ITransition<TContext> transition : m_aTransitionTable[m_iCurrentStateIdx])
		{
			if (transition.ShouldBePerformed(m_pContext, timeSlice))
			{
				transition.OnPerform(m_pContext);
				ChangeState(transition.GetToStateID());
				return;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ChangeState(ACE_FSM_EStateID newStateID)
	{
		foreach (int idx, ACE_FSM_IState<TContext> state : m_aStates)
		{
			if (newStateID == state.GetID())
				return ChangeStateByIdx(idx);
		}
		
		if (!newStateID)
			Stop();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ChangeStateByIdx(int newStateIdx)
	{
		if (m_iCurrentStateIdx >= 0)
			m_aStates[m_iCurrentStateIdx].OnExit(m_pContext);
		
		m_iCurrentStateIdx = newStateIdx;
		
		if (m_iCurrentStateIdx >= 0)
			m_aStates[m_iCurrentStateIdx].OnEnter(m_pContext);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetContext(TContext context)
	{
		m_pContext = context;
	}
	
	//------------------------------------------------------------------------------------------------
	TContext GetContext()
	{
		return m_pContext;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_FSM_EStateID GetCurrentStateID()
	{
		return m_aStates[m_iCurrentStateIdx];
	}
	
	//------------------------------------------------------------------------------------------------
	//! Makes a shallow copy of states and transitions of the based FSM
	override void CopyFrom(ACE_IFrameJob from)
	{
		super.CopyFrom(from);
		
		ACE_FSM_Machine<TContext> castedFrom = ACE_FSM_Machine<TContext>.Cast(from);
		if (!castedFrom)
			return;
		
		m_aStates = castedFrom.m_aStates;
		m_aTransitions = castedFrom.m_aTransitions;
		m_aTransitionTable = castedFrom.m_aTransitionTable;
	}
}
