//------------------------------------------------------------------------------------------------
//! System responsible for executing per frame handling of equipped weapons with ACE_WeaponGadgetComponent
class ACE_WeaponGadgetsSystem : GameSystem
{
	protected bool m_bUpdating = false;
	protected ref array<ACE_WeaponGadgetComponent> m_aComponents = {};
	protected ref array<ACE_WeaponGadgetComponent> m_aDeletedComponents = {};
	
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
		Enable(!m_aComponents.IsEmpty());
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdatePoint(WorldUpdatePointArgs args)
	{
		super.OnUpdatePoint(args);
		
		m_bUpdating = true;
		
		foreach (ACE_WeaponGadgetComponent component : m_aComponents)
		{
			component.OnWeaponUpdate(args.GetTimeSliceSeconds());
		}
		
		m_bUpdating = false;
		
		foreach (ACE_WeaponGadgetComponent component: m_aDeletedComponents)
		{
			Unregister(component);
		}
		
		m_aDeletedComponents.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull ACE_WeaponGadgetComponent component)
	{
		if (component.GetOwner().IsDeleted() || m_aComponents.Contains(component))
			return;
		
		m_aComponents.Insert(component);
		Enable(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(ACE_WeaponGadgetComponent component)
	{
		int idx = m_aComponents.Find(component);
		if (idx < 0)
			return;
		
		if (m_bUpdating)
		{
			m_aDeletedComponents.Insert(component);
		}
		else
		{
			m_aComponents.Remove(idx);
			Enable(!m_aComponents.IsEmpty());
		}
	}
}
