//------------------------------------------------------------------------------------------------
class ACE_CookOff_DetonateAmmoSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_CookOff_DetonateAmmo_JobScheduler m_pScheduler;
	
	//------------------------------------------------------------------------------------------------
	static ACE_CookOff_DetonateAmmoSystem GetInstance(ChimeraWorld world)
	{
		return ACE_CookOff_DetonateAmmoSystem.Cast(world.FindSystem(ACE_CookOff_DetonateAmmoSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Server)
			.AddPoint(WorldSystemPoint.FixedFrame);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Should be paused when the world is not running
	override bool ShouldBePaused()
	{
		return true;
	}
	
	override protected void OnInit()
	{
		m_pScheduler.OnInit(CreateTemplateJob());
		
		ACE_FrameJobSystem jobManager = ACE_FrameJobSystem.GetInstance();
		if (jobManager)
			jobManager.Register(m_pScheduler, ESystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	protected ACE_CookOff_DetonateAmmoJob CreateTemplateJob()
	{
		return new ACE_CookOff_DetonateAmmoJob();
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull IEntity object)
	{
		m_pScheduler.Register(object);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(IEntity object)
	{
		m_pScheduler.Unregister(object);
	}
}
