//------------------------------------------------------------------------------------------------
//! System that manages system frame jobs (ACE_IFrameJob)
class ACE_FrameJobSystem : GameSystem
{
	protected ref array<ref ACE_FrameJobSystem_IPointContext> m_aSystemPointContexts = {};
	
	//------------------------------------------------------------------------------------------------
	static ACE_FrameJobSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_FrameJobSystem.Cast(world.FindSystem(ACE_FrameJobSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Both)
			.AddPoint(WorldSystemPoint.Frame)
			.AddPoint(WorldSystemPoint.FixedFrame)
			.AddPoint(WorldSystemPoint.PostFixedFrame);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Should be paused when the world is not running
	override bool ShouldBePaused()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_FrameJobSystem()
	{
		int min, max;
		SCR_Enum.GetRange(ESystemPoint, min, max);
		m_aSystemPointContexts.Resize(max + 1);
		
		// Frame events
		m_aSystemPointContexts[ESystemPoint.Frame] = new ACE_FrameJobSystem_FramePointContext(this);
		
		// Fixed frame events
		m_aSystemPointContexts[ESystemPoint.FixedFrame] = new ACE_FrameJobSystem_FixedFramePointContext(this);
		m_aSystemPointContexts[ESystemPoint.PostFixedFrame] = new ACE_FrameJobSystem_FixedFramePointContext(this);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		CheckTermination();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Starts a system point job
	void Register(notnull ACE_IFrameJob job, ESystemPoint point)
	{
		m_aSystemPointContexts[point].Register(job);
		Enable(true);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminates a system point job
	void Unregister(notnull ACE_IFrameJob job, ESystemPoint point)
	{
		m_aSystemPointContexts[point].Unregister(job);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdate(ESystemPoint point)
	{
		m_aSystemPointContexts[point].OnUpdate();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check and execute termination of system if needed
	void CheckTermination()
	{
		foreach (ACE_FrameJobSystem_IPointContext context : m_aSystemPointContexts)
		{
			if (context && context.IsEnable())
				return;
		}
		
		Enable(false);
	}
}

//------------------------------------------------------------------------------------------------
class ACE_FrameJobSystem_IPointContext : Managed
{
	protected ACE_FrameJobSystem m_pSystem;
	protected ref array<ref ACE_IFrameJob> m_aJobs = {};
	protected bool m_bIsUpdating = false;
	protected bool m_bIsEnabled = false;
	
	//------------------------------------------------------------------------------------------------
	void ACE_FrameJobSystem_IPointContext(ACE_FrameJobSystem system)
	{
		m_pSystem = system;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetTimeSlice();
	
	//------------------------------------------------------------------------------------------------
	void OnUpdate()
	{
		float timeSlice = GetTimeSlice();
		m_bIsUpdating = true;
		
		foreach (ACE_IFrameJob job : m_aJobs)
		{
			if (job.ShouldBeUpdated())
				job.OnUpdate(timeSlice);
		}
		
		m_bIsUpdating = false;
		
		// Unregister jobs that should be terminated
		for (int i = m_aJobs.Count() - 1; i >= 0; --i)
		{
			ACE_IFrameJob job = m_aJobs[i];
			if (job.ShouldBeStopped())
				Unregister(job);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(ACE_IFrameJob job)
	{
		job.OnStart();
		m_aJobs.Insert(job);
		Enable(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(ACE_IFrameJob job)
	{
		if (m_bIsUpdating)
		{
			job.Stop();
		}
		else
		{
			m_aJobs.RemoveItem(job);
			job.OnStop();
			
			if (m_aJobs.IsEmpty())
			{
				Enable(false);
				m_pSystem.CheckTermination();
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Enable(bool enable)
	{
		m_bIsEnabled = enable;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsEnable()
	{
		return m_bIsEnabled;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_FrameJobSystem_FramePointContext : ACE_FrameJobSystem_IPointContext
{
	//------------------------------------------------------------------------------------------------
	override float GetTimeSlice()
	{
		return GetGame().GetWorld().GetTimeSlice();
	}
}

//------------------------------------------------------------------------------------------------
class ACE_FrameJobSystem_FixedFramePointContext : ACE_FrameJobSystem_IPointContext
{
	//------------------------------------------------------------------------------------------------
	override float GetTimeSlice()
	{
		return GetGame().GetWorld().GetFixedTimeSlice();
	}
}

//------------------------------------------------------------------------------------------------
class ACE_FrameJobSystem_PhysicsPointContext : ACE_FrameJobSystem_IPointContext
{
	//------------------------------------------------------------------------------------------------
	override float GetTimeSlice()
	{
		return GetGame().GetWorld().GetPhysicsTimeSlice();
	}
}
