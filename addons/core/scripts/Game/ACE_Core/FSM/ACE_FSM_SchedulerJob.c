//------------------------------------------------------------------------------------------------
//! Composite job that manages FSMs based on a template for a set of objects
[BaseContainerProps()]
class ACE_FSM_SchedulerJob<Class TObject, ACE_FSM_System_IObjectContext TContext, ACE_FSM_Machine TMachine> : ACE_IFrameJob
{
	[Attribute(defvalue: "1000", desc: "Approximate update timeout for a particular object [s]")]
	protected int m_fObjectUpdateTimeoutMS;

	[Attribute(defvalue: "10", desc: "Maximum number of entities to be updated per frame")]
	protected int m_iMaxIterationsPerFrame;
	
	protected ref TMachine m_pTemplateFSM;
	protected ref array<ref TMachine> m_aMachines = {};
	protected ref array<TObject> m_aInitialObjectsToRegister = {};
	protected ACE_FSM_Scheduler_EState m_eState = ACE_FSM_Scheduler_EState.UPDATE_QUEUE;
	protected float m_fNextUpdateStartTime = 0;
	protected int m_iCurrentMachineIdx = 0;
	protected int m_iNumMachines = 0;
	
	//------------------------------------------------------------------------------------------------
	//! Template is passed here rather than in ctor for usage of ACE_FSM_SchedulerJob in attributes
	void OnInit(TMachine templateFSM)
	{
		m_pTemplateFSM = templateFSM;
		
		foreach (TObject object : m_aInitialObjectsToRegister)
		{
			Register(object);
		}
		
		m_aInitialObjectsToRegister.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		super.OnUpdate(timeSlice);
		float currentTime = GetGame().GetWorld().GetWorldTime();
		
		switch (m_eState)
		{
			case ACE_FSM_Scheduler_EState.UPDATE_QUEUE: return UpdateQueueStep(currentTime);
			case ACE_FSM_Scheduler_EState.UPDATE_OBJECTS: return UpdateObjectsStep(currentTime);
			case ACE_FSM_Scheduler_EState.TIMEOUT: return TimeoutStep(currentTime);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateQueueStep(float currentTime)
	{
		m_iNumMachines = m_aMachines.Count();
		
		for (int i = m_iNumMachines - 1; i >= 0; --i)
		{
			TMachine machine = m_aMachines[i];
			if (machine.ShouldBeStopped())
				UnregisterNow(i);
		}
		
		m_iCurrentMachineIdx = 0;
		m_iNumMachines = m_aMachines.Count();
		
		if (m_iNumMachines == 0)
			return ToggleUpdate(false);
		
		m_fNextUpdateStartTime = currentTime + m_fObjectUpdateTimeoutMS;
		ChangeState(ACE_FSM_Scheduler_EState.UPDATE_OBJECTS);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateObjectsStep(float currentTime)
	{
		for (int i = 0; i < m_iMaxIterationsPerFrame; ++i)
		{
			TMachine machine = m_aMachines[m_iCurrentMachineIdx];
			TContext context = machine.GetContext();
			machine.OnUpdate((currentTime - context.m_fLastUpdateTime) / 1000);
			context.m_fLastUpdateTime = currentTime;
			++m_iCurrentMachineIdx;
			
			if (m_iCurrentMachineIdx >= m_iNumMachines)
				return ChangeState(ACE_FSM_Scheduler_EState.TIMEOUT);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void TimeoutStep(float currentTime)
	{
		if (currentTime >= m_fNextUpdateStartTime)
			ChangeState(ACE_FSM_Scheduler_EState.UPDATE_QUEUE);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ChangeState(ACE_FSM_Scheduler_EState newState)
	{
		m_eState = newState;
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(TObject object)
	{
		if (!m_pTemplateFSM)
		{
			m_aInitialObjectsToRegister.Insert(object);
			return;
		}
		
		TMachine machine = CreateFSM();
		machine.SetContext(new TContext(object));
		machine.OnStart();
		m_aMachines.Insert(machine);
		
		if (!ShouldBeUpdated())
			ToggleUpdate(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(TObject object)
	{
		foreach (int i, TMachine machine : m_aMachines)
		{
			if (machine.GetContext().m_pObject == object)
			{
				// Delay unregistration, unless the queue isn't touched (e.g. during timeout)
				if (m_eState == ACE_FSM_Scheduler_EState.TIMEOUT)
					UnregisterNow(i);
				else
					machine.Stop();
				
				return;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UnregisterNow(int machineIdx)
	{
		TMachine machine = m_aMachines[machineIdx];
		m_aMachines.Remove(machineIdx);
		machine.OnStop();
	}
	
	//------------------------------------------------------------------------------------------------
	protected TMachine CreateFSM()
	{
		TMachine instance = new TMachine();
		instance.CopyFrom(m_pTemplateFSM);
		return instance;
	}
}

//------------------------------------------------------------------------------------------------
enum ACE_FSM_Scheduler_EState
{
	UPDATE_QUEUE,
	UPDATE_OBJECTS,
	TIMEOUT
}

//------------------------------------------------------------------------------------------------
class ACE_FSM_System_IObjectContext<Class TObject>
{
	TObject m_pObject;
	float m_fLastUpdateTime;
	
	//------------------------------------------------------------------------------------------------
	void ACE_FSM_System_IObjectContext(TObject object)
	{
		m_pObject = object;
		m_fLastUpdateTime = GetGame().GetWorld().GetWorldTime();
	}
}
