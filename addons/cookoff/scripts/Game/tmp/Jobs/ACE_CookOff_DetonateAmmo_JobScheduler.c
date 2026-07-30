//------------------------------------------------------------------------------------------------
//! Manual template initialized copy of
//! ACE_TemplateFrameJobScheduler<IEntity, ACE_CookOff_ObjectContext, ACE_CookOff_DetonateAmmoJob>
//! as workaround for:
//! - https://feedback.bistudio.com/T193122
//! - https://feedback.bistudio.com/T193389
[BaseContainerProps()]
class ACE_CookOff_DetonateAmmo_JobScheduler : ACE_IFrameJob
{
	[Attribute(defvalue: "1000", desc: "Approximate update timeout for a particular object [ms]")]
	protected int m_fObjectUpdateTimeoutMS;

	[Attribute(defvalue: "10", desc: "Maximum number of entities to be updated per frame")]
	protected int m_iMaxIterationsPerFrame;
	
	protected ref ACE_CookOff_DetonateAmmoJob m_pTemplateJob;
	protected ref array<ref ACE_CookOff_DetonateAmmoJob> m_aJobs = {};
	protected ref array<IEntity> m_aInitialObjectsToRegister = {};
	protected ACE_FrameJobScheduler_EState m_eState = ACE_FrameJobScheduler_EState.UPDATE_QUEUE;
	protected float m_fNextUpdateStartTime = 0;
	protected int m_iCurrentJobIdx = 0;
	protected int m_iNumMachines = 0;
	
	//------------------------------------------------------------------------------------------------
	//! Template is passed here rather than in ctor for usage of ACE_TemplateFrameJobScheduler in attributes
	void OnInit(ACE_CookOff_DetonateAmmoJob templateJob)
	{
		m_pTemplateJob = templateJob;
		
		foreach (IEntity object : m_aInitialObjectsToRegister)
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
			ACE_CookOff_DetonateAmmoJob job = m_aJobs[i];
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
			ACE_CookOff_DetonateAmmoJob job = m_aJobs[m_iCurrentJobIdx];
			ACE_CookOff_ObjectContext context = job.GetContext();
			
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
	void Register(IEntity object)
	{
		if (!m_pTemplateJob)
		{
			m_aInitialObjectsToRegister.Insert(object);
			return;
		}
		
		ACE_CookOff_ObjectContext context = new ACE_CookOff_ObjectContext(object);
		if (!context.IsValid())
			return;
		
		ACE_CookOff_DetonateAmmoJob job = CreateJob();
		job.SetContext(context);
		job.OnStart();
		m_aJobs.Insert(job);
		
		if (!ShouldBeUpdated())
			ToggleUpdate(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(IEntity object)
	{
		foreach (int i, ACE_CookOff_DetonateAmmoJob job : m_aJobs)
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
		ACE_CookOff_DetonateAmmoJob job = m_aJobs[machineIdx];
		m_aJobs.Remove(machineIdx);
		job.OnStop();
	}
	
	//------------------------------------------------------------------------------------------------
	protected ACE_CookOff_DetonateAmmoJob CreateJob()
	{
		ACE_CookOff_DetonateAmmoJob instance = new ACE_CookOff_DetonateAmmoJob();
		instance.CopyFrom(m_pTemplateJob);
		return instance;
	}
}
