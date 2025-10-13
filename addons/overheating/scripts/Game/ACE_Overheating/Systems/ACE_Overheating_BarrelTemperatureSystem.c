//------------------------------------------------------------------------------------------------
class ACE_Overheating_BarrelTemperatureSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_Overheating_BarrelTemperature_JobScheduler m_pScheduler;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_BarrelTemperatureSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_Overheating_BarrelTemperatureSystem.Cast(world.FindSystem(ACE_Overheating_BarrelTemperatureSystem));
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
	protected ACE_Overheating_BarrelTemperatureJob CreateTemplateJob()
	{
		return new ACE_Overheating_BarrelTemperatureJob();
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull ACE_Overheating_BarrelComponent barrel)
	{
		m_pScheduler.Register(barrel);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(ACE_Overheating_BarrelComponent barrel)
	{
		m_pScheduler.Unregister(barrel);
	}
}
