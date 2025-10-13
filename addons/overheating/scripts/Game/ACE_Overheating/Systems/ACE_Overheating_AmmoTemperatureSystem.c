//------------------------------------------------------------------------------------------------
class ACE_Overheating_AmmoTemperatureSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_Overheating_AmmoTemperature_JobScheduler m_pScheduler;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_AmmoTemperatureSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_Overheating_AmmoTemperatureSystem.Cast(world.FindSystem(ACE_Overheating_AmmoTemperatureSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Server)
			.AddPoint(WorldSystemPoint.BeforeEntitiesCreated);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Should be paused when the world is not running
	override bool ShouldBePaused()
	{
		return true;
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
	protected ACE_Overheating_AmmoTemperatureJob CreateTemplateJob()
	{
		return new ACE_Overheating_AmmoTemperatureJob();
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
