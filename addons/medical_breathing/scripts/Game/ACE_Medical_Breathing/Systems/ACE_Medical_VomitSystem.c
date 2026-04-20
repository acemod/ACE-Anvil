//------------------------------------------------------------------------------------------------
//! Responsible for handling occlusion of airway
class ACE_Medical_VomitSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_Medical_Vomit_JobScheduler m_pScheduler;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_VomitSystem GetInstance(ChimeraWorld world)
	{
		return ACE_Medical_VomitSystem.Cast(world.FindSystem(ACE_Medical_VomitSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Should be paused when the world is not running
	override bool ShouldBePaused()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(ESystemLocation.Server);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		m_pScheduler.OnInit(CreateTemplateJob());
		
		ACE_FrameJobSystem jobManager = ACE_FrameJobSystem.GetInstance();
		if (jobManager)
			jobManager.Register(m_pScheduler, ESystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	protected ACE_Medical_VomitJob CreateTemplateJob()
	{
		return new ACE_Medical_VomitJob();
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull SCR_ChimeraCharacter char)
	{
		m_pScheduler.Register(char);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(SCR_ChimeraCharacter char)
	{
		m_pScheduler.Unregister(char);
	}
}
