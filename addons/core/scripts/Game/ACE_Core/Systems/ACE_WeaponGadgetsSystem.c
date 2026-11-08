//------------------------------------------------------------------------------------------------
//! System responsible for executing per frame handling of equipped weapons with ACE_WeaponGadgetComponent
class ACE_WeaponGadgetsSystem : GameSystem
{
	protected ACE_WeaponGadgetComponent m_CurrentGadgetComponent;
	
	//------------------------------------------------------------------------------------------------
	static ACE_WeaponGadgetsSystem GetInstance(ChimeraWorld world)
	{
		return ACE_WeaponGadgetsSystem.Cast(world.FindSystem(ACE_WeaponGadgetsSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Both)
			.AddPoint(WorldSystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		Enable(m_CurrentGadgetComponent != null);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdatePoint(WorldUpdatePointArgs args)
	{
		super.OnUpdatePoint(args);
		m_CurrentGadgetComponent.OnWeaponUpdate(args.GetTimeSliceSeconds());
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(ACE_WeaponGadgetComponent component)
	{
		m_CurrentGadgetComponent = component;
		Enable(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(ACE_WeaponGadgetComponent component)
	{
		m_CurrentGadgetComponent = null;
		Enable(false);
	}
}
