//------------------------------------------------------------------------------------------------
//! Responsible for handling pneumothorax and deterioration to tension pneumothorax
class ACE_Medical_PneumothoraxSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_Medical_Pneumothorax_JobScheduler m_pScheduler;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_PneumothoraxSystem GetInstance(ChimeraWorld world)
	{
		return ACE_Medical_PneumothoraxSystem.Cast(world.FindSystem(ACE_Medical_PneumothoraxSystem));
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
	protected ACE_Medical_PneumothoraxJob CreateTemplateJob()
	{
		return new ACE_Medical_PneumothoraxJob();
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
