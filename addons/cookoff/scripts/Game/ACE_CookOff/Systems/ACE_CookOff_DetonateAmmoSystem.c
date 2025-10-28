//------------------------------------------------------------------------------------------------
class ACE_CookOff_DetonateAmmoSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_CookOff_DetonateAmmo_JobScheduler m_pScheduler;
	
	//------------------------------------------------------------------------------------------------
	static ACE_CookOff_DetonateAmmoSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_CookOff_DetonateAmmoSystem.Cast(world.FindSystem(ACE_CookOff_DetonateAmmoSystem));
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
