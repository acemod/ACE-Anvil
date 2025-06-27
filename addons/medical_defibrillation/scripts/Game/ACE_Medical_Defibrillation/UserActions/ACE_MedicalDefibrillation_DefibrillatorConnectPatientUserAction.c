class ACE_MedicalDefibrillation_DefibrillationConnectPatientUserAction: ScriptedUserAction
{
	protected float m_fLastUpdateTime;
	protected float m_fSearchDistancePerform = 3;
	protected float m_fSearchDistanceShown = 3;
	protected float m_fShortestDistancePerform;
	protected float m_fShortestDistanceShown;
	
	protected IEntity m_NearestDefibPerform;
	protected IEntity m_NearestDefibShown;
	
	protected vector m_vSearchPosPerform;
	protected vector m_vSearchPosShown;
	
	protected const float UPDATERATE = 500;
	
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
	//! Make the action server only
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript() { return false; }
	override bool HasLocalEffectOnlyScript() { return false; }
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{	
		super.CanBeShownScript(user);
		
		IEntity patient = IEntity.Cast(GetOwner());
		if (!patient)
			return false;
		
		// slow down tick rate
		if (!CanExecuteThisTick())
		{
			return (m_NearestDefibShown && !DefibAlreadyFoundAndConnected());
		}
		
		// check for nearby defib
		QueryForDefibShown(patient.GetOrigin(), 3);
		if (!m_NearestDefibShown)
		{
			PrintFormat("%1::CanBeShownScript | No defibrillator found!", this.ClassName(), level: LogLevel.WARNING);
			return false;
		}
		
		// check for defib component
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(m_NearestDefibShown.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		// check for connected patient
		if (defibrillatorComponent.GetConnectedPatient())
		{
			PrintFormat("%1::CanBeShownScript | Patient already connected!", this.ClassName(), level: LogLevel.WARNING);
			return false;
		}
		
		PrintFormat("%1::CanBeShownScript | Defib found and ready for patient", this.ClassName());

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		PrintFormat("%1::Perform Action | Server Execution: %2", this.ClassName(), Replication.IsServer());
		
		// check for neaby defib
		QueryForDefibPerform(pOwnerEntity.GetOrigin(), 3);
		if (!m_NearestDefibPerform)
		{
			PrintFormat("%1::PerformAction | No Defibrillator Found!", this.ClassName(), level: LogLevel.WARNING);
			return;
		}
		
		// check for defib compponent
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(m_NearestDefibPerform.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		// connect and notify
		if (defibrillatorComponent.ConnectPatient(pOwnerEntity))
		{
			ACE_MedicalDefibrillation_NetworkComponent networkComponent;
			if (!ACE_MedicalDefibrillator_DefibrillatorBaseUserAction.GetDefibrillatorNetworkComponent(pUserEntity, networkComponent))
				return;
			
			if (EntityUtils.IsPlayer(pOwnerEntity))
				networkComponent.RequestDefibrillatorNotification(ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTCONNECTED, defibrillatorComponent.GetOwner(), SCR_ChimeraCharacter.Cast(defibrillatorComponent.GetConnectedPatient()));
			else
				networkComponent.RequestDefibrillatorNotification(ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTCONNECTED_AI, defibrillatorComponent.GetOwner(), SCR_ChimeraCharacter.Cast(defibrillatorComponent.GetConnectedPatient()));
		}
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
	protected bool DefibAlreadyFoundAndConnected()
	{
		if (!m_NearestDefibShown)
			return false;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(m_NearestDefibShown.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		if (!defibrillatorComponent.GetConnectedPatient())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool IsAlive(IEntity entity)
	{
		if (!entity)
			return false;
		
		DamageManagerComponent damageManager;
		ChimeraCharacter character = ChimeraCharacter.Cast(entity);
		if (character)
			damageManager = character.GetDamageManager();
		else
			damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		
		if (!damageManager)
			return true;
		
		return damageManager.GetState() != EDamageState.DESTROYED;
	}
	
	//------------------------------------------------------------------------------------------------
	// QUERY SYSTEM	
	//------------------------------------------------------------------------------------------------
	protected void QueryForDefibShown(vector searchPos, float searchDistance)
	{
		m_NearestDefibShown = null;
		m_vSearchPosShown = searchPos;
		m_fSearchDistanceShown = searchDistance;
		
		m_fShortestDistanceShown = m_fSearchDistanceShown;
		
		GetGame().GetWorld().QueryEntitiesBySphere(m_vSearchPosShown, m_fSearchDistanceShown, QueryForDefibShownCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void QueryForDefibPerform(vector searchPos, float searchDistance)
	{
		m_NearestDefibPerform = null;
		m_vSearchPosPerform = searchPos;
		m_fSearchDistancePerform = searchDistance;
		
		m_fShortestDistancePerform = m_fSearchDistancePerform;
		
		GetGame().GetWorld().QueryEntitiesBySphere(m_vSearchPosPerform, m_fSearchDistancePerform, QueryForDefibPerformCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool QueryForDefibShownCallback(IEntity entity)
	{
		if (!entity)
			return true;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return true;
		
		if (defibrillatorComponent.GetConnectedPatient())
			return true;
		
		float distance = vector.Distance(m_vSearchPosShown, entity.GetOrigin());
		
		if (m_fShortestDistanceShown > distance)
		{
			m_fShortestDistanceShown = distance;
			m_NearestDefibShown = entity;
		}
		
		return (m_fShortestDistanceShown != 0);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool QueryForDefibPerformCallback(IEntity entity)
	{
		if (!entity)
			return true;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return true;

		if (defibrillatorComponent.GetConnectedPatient())
			return true;
		
		float distance = vector.Distance(m_vSearchPosPerform, entity.GetOrigin());
		
		if (m_fShortestDistancePerform > distance)
		{
			m_fShortestDistancePerform = distance;
			m_NearestDefibPerform = entity;
		}
		
		return (m_fShortestDistancePerform != 0);
	}
}