//------------------------------------------------------------------------------------------------
class ACE_BaseSystem : GameSystem
{
	[Attribute(defvalue: "1000", desc: "Approximate update timeout for a particular entity [s]")]
	protected int m_fEntityUpdateTimeoutMS;

	[Attribute(defvalue: "10", desc: "Maximum number of entities to be updated per frame")]
	protected int m_iMaxIterationsPerFrame;
	
	protected ACE_BaseSystem_EState m_eCurrentState = ACE_BaseSystem_EState.QUEUE_UPDATE;
	protected ref array<ref ACE_BaseSystem_EntityContext> m_aEntityQueue = {};
	protected ref array<IEntity> m_aEntitiesToRegister = {};
	protected ref array<IEntity> m_aEntitiesToUnregister = {};
	protected float m_fNextUpdateStartTime = 0;
	protected int m_iCurrentEntityIdx = 0;
	protected int m_iNumEntities = 0;
		
	//------------------------------------------------------------------------------------------------
	static ACE_BaseSystem GetInstance(typename type)
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_BaseSystem.Cast(world.FindSystem(type));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(true)
			.SetUnique(false)
			.SetLocation(ESystemLocation.Server)
			.AddPoint(ESystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Do not run when game is paused
	override bool ShouldBePaused()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnRegistered(ACE_BaseSystem_EntityContext context);
	
	//------------------------------------------------------------------------------------------------
	//! Make sure to handle the case where entity is null, as it can already be deleted by the time
	//! the queue update takes place that calls onUnregistered
	void OnUnregistered(ACE_BaseSystem_EntityContext context);
	
	//------------------------------------------------------------------------------------------------
	//! Three states: QUEUE_UPDATE -> ENTITY_UPDATE -> TIMEOUT
	override protected void OnUpdate(ESystemPoint point)
	{
		super.OnUpdate(point);
		float currentTime = GetWorld().GetWorldTime();
		
		switch (m_eCurrentState)
		{
			case ACE_BaseSystem_EState.TIMEOUT:
			{
				if (currentTime >= m_fNextUpdateStartTime)
				{
					m_eCurrentState = ACE_BaseSystem_EState.QUEUE_UPDATE;
					m_fNextUpdateStartTime = currentTime + m_fEntityUpdateTimeoutMS;
				}
				
				return;
			}
			
			case ACE_BaseSystem_EState.ENTITY_UPDATE:
			{
				for (int i = 0; i < m_iMaxIterationsPerFrame; ++i)
				{
					ACE_BaseSystem_EntityContext context = m_aEntityQueue[m_iCurrentEntityIdx];
					UpdateEntity(context, (currentTime - context.m_fLastUpdateTime) / 1000);
					context.m_fLastUpdateTime = currentTime;
					++m_iCurrentEntityIdx;
					
					if (m_iCurrentEntityIdx >= m_iNumEntities)
						break;
				}
				
				if (m_iCurrentEntityIdx >= m_iNumEntities)
					m_eCurrentState = ACE_BaseSystem_EState.TIMEOUT;
				
				return;
			}
			
			case ACE_BaseSystem_EState.QUEUE_UPDATE:
			{
				m_aEntityQueue.Reserve(m_aEntityQueue.Count() + m_aEntitiesToRegister.Count());
				
				foreach (IEntity entity : m_aEntitiesToRegister)
				{
					Register(entity);
				}
				
				foreach (IEntity entity : m_aEntitiesToUnregister)
				{
					Unregister(entity);
				}
				
				m_aEntitiesToRegister.Clear();
				m_aEntitiesToUnregister.Clear();
				m_eCurrentState = ACE_BaseSystem_EState.ENTITY_UPDATE;
				m_iCurrentEntityIdx = 0;
				m_iNumEntities = m_aEntityQueue.Count();
				Enable(m_iNumEntities > 0);
				return;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateEntity(ACE_BaseSystem_EntityContext context, float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull IEntity entity)
	{
		// Only directly push back to queue when entities are not getting updated
		if (m_eCurrentState == ACE_BaseSystem_EState.ENTITY_UPDATE)
		{
			m_aEntitiesToRegister.Insert(entity);
			m_aEntitiesToUnregister.RemoveItem(entity);
			
			if (!IsEnabled())
			{
				m_eCurrentState = ACE_BaseSystem_EState.QUEUE_UPDATE;
				Enable(true);
			}
		}
		else
		{
			ACE_BaseSystem_EntityContext context = CreateEntityContext(entity);
			m_aEntityQueue.Insert(context);
			OnRegistered(context);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(IEntity entity)
	{
		// Only directly remove from queue when entities are not getting updated
		if (m_eCurrentState == ACE_BaseSystem_EState.ENTITY_UPDATE)
		{
			m_aEntitiesToUnregister.Insert(entity);
			m_aEntitiesToRegister.RemoveItem(entity);
		}
		else
		{
			foreach (int i, ACE_BaseSystem_EntityContext context : m_aEntityQueue)
			{
				if (context.m_pEntity != entity)
					continue;
				
				OnUnregistered(context);
				m_aEntityQueue.Remove(i);
				break;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Factory method for entity contexts
	protected ACE_BaseSystem_EntityContext CreateEntityContext(IEntity entity)
	{
		return new ACE_BaseSystem_EntityContext(entity);
	}
}
