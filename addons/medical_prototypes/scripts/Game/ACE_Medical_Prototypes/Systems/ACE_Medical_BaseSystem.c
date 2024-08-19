//------------------------------------------------------------------------------------------------
//! Each medical system has an associated component it operrates on, e.g. ACE_Medical_MedicationSystem
//! updates the medication data on ACE_Medical_MedicationComponent for all entities
class ACE_Medical_BaseSystem : GameSystem
{
	[Attribute(defvalue: "1000", desc: "Minimum update timeout for a particular entity [ms]")]
	protected int m_fMinEntityUpdateTimeoutMS;

	[Attribute(defvalue: "10", desc: "Maximum number of entities to process in an update")]
	protected int m_iMaxIterationsPerUpdate;
	
	protected bool m_bSystemInitDone = false;
	
	protected ref SCR_SortedArray<IEntity> m_aQueue = new SCR_SortedArray<IEntity>();
		
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_BaseSystem GetInstance(typename type)
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_Medical_BaseSystem.Cast(world.FindSystem(type));
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		
		// Turn off if the queue is currently empty
		if (m_aQueue.IsEmpty())
			Enable(false);
		
		for (int i = 0; i < m_aQueue.Count(); i++)
		{
			OnStart(m_aQueue[i]);
		}
		
		m_bSystemInitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnStart(IEntity entity);
	
	//------------------------------------------------------------------------------------------------
	void OnStop(IEntity entity);
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdate(ESystemPoint point)
	{
		super.OnUpdate(point);
		int currentTime = System.GetTickCount();
		
		for (int i = 0; i < m_iMaxIterationsPerUpdate; i++)
		{
			int lastUpdateTime = m_aQueue.GetOrder(0);
			
			if (lastUpdateTime + m_fMinEntityUpdateTimeoutMS > currentTime)
				return;
			
			IEntity entity = m_aQueue.GetValue(0);
			Update(entity, System.GetTickCount(lastUpdateTime) / 1000);
			m_aQueue.Remove(0);
			m_aQueue.Insert(currentTime, entity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Update(IEntity entity, float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	void OnFullHeal(IEntity entity);
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull IEntity entity)
	{
		if (m_aQueue.Contains(entity))
			return;
		
		if (!IsEnabled())
			Enable(true);
		
		if (m_bSystemInitDone)
			OnStart(entity);
		
		m_aQueue.Insert(System.GetTickCount(), entity);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(notnull IEntity entity)
	{
		m_aQueue.RemoveValues(entity);
		
		if (m_aQueue.IsEmpty())
			Enable(false);
	}
}
