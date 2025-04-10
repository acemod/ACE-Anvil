//------------------------------------------------------------------------------------------------
//! Each medical system has an associated component it operrates on, e.g. ACE_Medical_MedicationSystem
//! updates the medication data on ACE_Medical_MedicationComponent for all entities
class ACE_Medical_BaseSystem : GameSystem
{
	[Attribute(defvalue: "1000", desc: "Minimum update timeout for a particular entity [ms]")]
	protected int m_fMinEntityUpdateTimeoutMS;

	[Attribute(defvalue: "10", desc: "Maximum number of entities to process in an update")]
	protected int m_iMaxIterationsPerUpdate;
	
	protected ref SCR_SortedArray<SCR_ChimeraCharacter> m_aQueue = new SCR_SortedArray<SCR_ChimeraCharacter>();
	protected ref array<SCR_ChimeraCharacter> m_aEntitiesToRegister = {};
	protected ref array<SCR_ChimeraCharacter> m_aEntitiesToUnregister = {};
	protected bool m_bInitDone = false;
	protected bool m_bIsUpdating = false;
	protected bool m_bScheduledQueueUpdate = false;
		
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_BaseSystem GetInstance(typename type)
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_Medical_BaseSystem.Cast(world.FindSystem(type));
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
		
		m_bInitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnStart(SCR_ChimeraCharacter entity);
	
	//------------------------------------------------------------------------------------------------
	//! Make sure to handle the case where entity is null, as it can already be deleted by the time
	//! the queue update takes place that calls OnStop
	void OnStop(SCR_ChimeraCharacter entity);
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdate(ESystemPoint point)
	{
		super.OnUpdate(point);
		
		// Set updating lock such that m_aQueue is left untouched by Register/Unregister
		m_bIsUpdating = true;
		float currentTime = GetWorld().GetWorldTime();
		
		for (int i = 0; i < m_iMaxIterationsPerUpdate; i++)
		{
			float lastUpdateTime = m_aQueue.GetOrder(0);
			
			if (lastUpdateTime + m_fMinEntityUpdateTimeoutMS > currentTime)
				break;
			
			SCR_ChimeraCharacter entity = m_aQueue.GetValue(0);
			Update(entity, (GetWorld().GetWorldTime() - lastUpdateTime) / 1000);
			m_aQueue.Remove(0);
			m_aQueue.Insert(currentTime, entity);
		}
		
		// Unset updating lock
		m_bIsUpdating = false;
		
		// Process entities scheduled for registering/unregistering from the queue
		if (m_bScheduledQueueUpdate)
		{
			foreach (SCR_ChimeraCharacter entity : m_aEntitiesToRegister)
			{
				Register(entity);
			}
	
			foreach (SCR_ChimeraCharacter entity : m_aEntitiesToUnregister)
			{
				Unregister(entity);
			}		
			
			m_aEntitiesToRegister.Clear();
			m_aEntitiesToUnregister.Clear();
			m_bScheduledQueueUpdate = false;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Update(SCR_ChimeraCharacter entity, float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	void OnFullHeal(SCR_ChimeraCharacter entity);
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull SCR_ChimeraCharacter entity)
	{
		// Only directly touch m_aQueue while no updating is going on
		if (m_bIsUpdating)
		{
			m_aEntitiesToRegister.Insert(entity);
			m_aEntitiesToUnregister.RemoveItem(entity);
			m_bScheduledQueueUpdate = true;
		}
		else
		{
			m_aQueue.Insert(GetWorld().GetWorldTime(), entity);
		
			if (m_bInitDone)
				OnStart(entity);
		
			if (!IsEnabled())
				Enable(true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(SCR_ChimeraCharacter entity)
	{
		// Only directly touch m_aQueue while no updating is going on
		if (m_bIsUpdating)
		{
			m_aEntitiesToUnregister.Insert(entity);
			m_aEntitiesToRegister.RemoveItem(entity);
			m_bScheduledQueueUpdate = true;
		}
		else
		{
			m_aQueue.RemoveValues(entity);
			OnStop(entity);
			
			if (m_aQueue.IsEmpty())
				Enable(false);
		}
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	//! Get target for diag menu and string for printing which target it is
	//! Return true if a target was found
	protected bool GetDiagTarget(out SCR_ChimeraCharacter target, out string targetType)
	{
		CameraBase camera = GetGame().GetCameraManager().CurrentCamera();
		if (!camera)
			return false;
				
		target = SCR_ChimeraCharacter.Cast(camera.GetCursorTarget());
		if (target)
		{
			targetType = "Target";
			return true;
		}
		
		target = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (target)
		{
			
			targetType = "Self";
			return true;
		}
		
		return false;
	}
#endif
}
