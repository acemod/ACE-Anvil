//------------------------------------------------------------------------------------------------
//! Client-side system for registering active pointers and adds them to the client's map
class ACE_Finger_MapPointingSystem : GameSystem
{
	protected ACE_Finger_MapUIPointerContainer m_pContainer;
	protected ref array<ACE_Finger_MapPointer> m_aActivePointers = {};
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetLocation(ESystemLocation.Client)
            .AddController(ACE_Finger_MapPointer);
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_Finger_MapPointingSystem GetInstance()
	{
		World world = GetGame().GetWorld();
		if (!world)
			return null;

		return ACE_Finger_MapPointingSystem.Cast(world.FindSystem(ACE_Finger_MapPointingSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterActivePointer(ACE_Finger_MapPointer ptr)
	{
		if (m_pContainer)
			m_pContainer.AddPointer(ptr);
			
		m_aActivePointers.Insert(ptr);
	}
		
	//------------------------------------------------------------------------------------------------
	void UnregisterActivePointer(ACE_Finger_MapPointer ptr)
	{
		if (m_pContainer)
			m_pContainer.RemovePointer(ptr);
			
		m_aActivePointers.RemoveItem(ptr);
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterContainer(ACE_Finger_MapUIPointerContainer container)
	{
		m_pContainer = container;
		
		foreach (ACE_Finger_MapPointer ptr : m_aActivePointers)
		{
			m_pContainer.AddPointer(ptr);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterContainer(ACE_Finger_MapUIPointerContainer container)
	{
		m_pContainer.ClearPointers();
		m_pContainer = null;
	}
}
