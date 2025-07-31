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
	protected ACE_Overheating_BarrelTemperatureJob CreateTemplateJob()
	{
		return new ACE_Overheating_BarrelTemperatureJob();
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull ACE_Overheating_MuzzleJamComponent jamComponent)
	{
		m_pScheduler.Register(jamComponent);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(ACE_Overheating_MuzzleJamComponent jamComponent)
	{
		m_pScheduler.Unregister(jamComponent);
	}
}
