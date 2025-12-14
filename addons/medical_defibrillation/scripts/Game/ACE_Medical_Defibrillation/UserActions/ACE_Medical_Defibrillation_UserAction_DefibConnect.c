class ACE_Medical_Defibrillation_UserActions_DefibConnect : ScriptedUserAction
{
	protected float m_fLastUpdateTime;
	
	protected const float UPDATERATE = 500;
	
	protected IEntity m_pNearestDefib
	
	//------------------------------------------------------------------------------------------------	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		World world = GetGame().GetWorld();
		if (!world)
			return;
		
		m_fLastUpdateTime = world.GetWorldTime();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		super.CanBeShownScript(user);
		
		if (!CanExecuteThisTick())
			return (m_pNearestDefib && !PriorDefibFound());
		
		PrintFormat("%1::CanBeShownScript | Checking for nearby defib...", this.ClassName());
		
		ACE_Medical_Defibrillation_QueryNearestDefib query = new ACE_Medical_Defibrillation_QueryNearestDefib(3);
		m_pNearestDefib = null;
		m_pNearestDefib = query.GetEntity(GetOwner().GetOrigin());
		if (!m_pNearestDefib)
		{
			PrintFormat("%1::CanBeShownScript | No nearby defib found!", this.ClassName());
			return false;
		}
		
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(m_pNearestDefib.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibComponent)
			return false;
		
		IEntity patient = defibComponent.GetPatient();
		if (patient)
		{
			PrintFormat("%1::CanBeShownScript | Defib already has a patient!", this.ClassName());
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		PrintFormat("%1::PerformAction | Server Execution: %2", this.ClassName(), Replication.IsServer());
		
		if (!m_pNearestDefib)
			return;
		
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(m_pNearestDefib.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibComponent)
			return;
		
		defibComponent.SetPatient(pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanExecuteThisTick()
	{
		float currentTime = GetGame().GetWorld().GetWorldTime();
		
		if (m_fLastUpdateTime + UPDATERATE > currentTime)
			return false;
		
		m_fLastUpdateTime = currentTime;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool PriorDefibFound()
	{
		if (!m_pNearestDefib)
			return false;
		
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(m_pNearestDefib.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibComponent)
			return false;
		
		IEntity patient = defibComponent.GetPatient();
		if (!patient)
			return false;
		
		return true;
	}
}