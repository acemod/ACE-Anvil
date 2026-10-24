//------------------------------------------------------------------------------------------------
//! Manual template initialized copy of
//! ACE_TemplateFrameJobScheduler<SCR_ChimeraCharacter, ACE_Medical_CharacterContext, ACE_Medical_VitalStateMachine>
//! as workaround for:
//! - https://feedback.bistudio.com/T193122
//! - https://feedback.bistudio.com/T193389
[BaseContainerProps()]
class ACE_Medical_VitalStates_JobScheduler : ACE_IFrameJob
{
	[Attribute(defvalue: "1000", desc: "Approximate update timeout for a particular object [ms]")]
	protected int m_fObjectUpdateTimeoutMS;

	[Attribute(defvalue: "10", desc: "Maximum number of entities to be updated per frame")]
	protected int m_iMaxIterationsPerFrame;
	
	protected ref ACE_Medical_VitalStateMachine m_pTemplateJob;
	protected ref array<ref ACE_Medical_VitalStateMachine> m_aJobs = {};
	protected ref array<SCR_ChimeraCharacter> m_aInitialObjectsToRegister = {};
	protected ACE_FrameJobScheduler_EState m_eState = ACE_FrameJobScheduler_EState.UPDATE_QUEUE;
	protected float m_fNextUpdateStartTime = 0;
	protected int m_iCurrentJobIdx = 0;
	protected int m_iNumMachines = 0;
	
	//------------------------------------------------------------------------------------------------
	//! Template is passed here rather than in ctor for usage of ACE_TemplateFrameJobScheduler in attributes
	void OnInit(ACE_Medical_VitalStateMachine templateJob)
	{
		m_pTemplateJob = templateJob;
		
		foreach (SCR_ChimeraCharacter object : m_aInitialObjectsToRegister)
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
			case ACE_FrameJobScheduler_EState.UPDATE_QUEUE: return UpdateQueueStep(currentTime);
			case ACE_FrameJobScheduler_EState.UPDATE_OBJECTS: return UpdateObjectsStep(currentTime);
			case ACE_FrameJobScheduler_EState.TIMEOUT: return TimeoutStep(currentTime);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateQueueStep(float currentTime)
	{
		m_iNumMachines = m_aJobs.Count();
		
		for (int i = m_iNumMachines - 1; i >= 0; --i)
		{
			ACE_Medical_VitalStateMachine job = m_aJobs[i];
			if (job.ShouldBeStopped())
				UnregisterNow(i);
		}
		
		m_iCurrentJobIdx = 0;
		m_iNumMachines = m_aJobs.Count();
		
		if (m_iNumMachines == 0)
			return ToggleUpdate(false);
		
		m_fNextUpdateStartTime = currentTime + m_fObjectUpdateTimeoutMS;
		ChangeState(ACE_FrameJobScheduler_EState.UPDATE_OBJECTS);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateObjectsStep(float currentTime)
	{
		for (int i = 0; i < m_iMaxIterationsPerFrame; ++i)
		{
			ACE_Medical_VitalStateMachine job = m_aJobs[m_iCurrentJobIdx];
			ACE_Medical_CharacterContext context = job.GetContext();
			
			if (context.IsValid())
				job.OnUpdate((currentTime - context.m_fLastUpdateTime) / 1000);
			else
				job.Stop();
			
			context.m_fLastUpdateTime = currentTime;
			++m_iCurrentJobIdx;
			
			if (m_iCurrentJobIdx >= m_iNumMachines)
				return ChangeState(ACE_FrameJobScheduler_EState.TIMEOUT);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void TimeoutStep(float currentTime)
	{
		if (currentTime >= m_fNextUpdateStartTime)
			ChangeState(ACE_FrameJobScheduler_EState.UPDATE_QUEUE);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ChangeState(ACE_FrameJobScheduler_EState newState)
	{
		m_eState = newState;
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(SCR_ChimeraCharacter object)
	{
		if (!m_pTemplateJob)
		{
			m_aInitialObjectsToRegister.Insert(object);
			return;
		}
		
		ACE_Medical_CharacterContext context = new ACE_Medical_CharacterContext(object);
		if (!context.IsValid())
			return;
		
		ACE_Medical_VitalStateMachine job = CreateJob();
		job.SetContext(context);
		job.OnStart();
		m_aJobs.Insert(job);
		
		if (!ShouldBeUpdated())
			ToggleUpdate(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(SCR_ChimeraCharacter object)
	{
		foreach (int i, ACE_Medical_VitalStateMachine job : m_aJobs)
		{
			if (job.GetContext().m_pObject == object)
			{
				// Delay unregistration, unless the queue isn't touched (e.g. during timeout)
				if (m_eState == ACE_FrameJobScheduler_EState.TIMEOUT)
					UnregisterNow(i);
				else
					job.Stop();
				
				return;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void UnregisterNow(int machineIdx)
	{
		ACE_Medical_VitalStateMachine job = m_aJobs[machineIdx];
		m_aJobs.Remove(machineIdx);
		job.OnStop();
	}
	
	//------------------------------------------------------------------------------------------------
	protected ACE_Medical_VitalStateMachine CreateJob()
	{
		ACE_Medical_VitalStateMachine instance = new ACE_Medical_VitalStateMachine();
		instance.CopyFrom(m_pTemplateJob);
		return instance;
	}
}
