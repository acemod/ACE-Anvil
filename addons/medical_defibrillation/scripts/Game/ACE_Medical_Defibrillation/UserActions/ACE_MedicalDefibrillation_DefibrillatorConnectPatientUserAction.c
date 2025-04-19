class ACE_MedicalDefibrillation_DefibrillationConnectPatientUserAction: ScriptedUserAction
{
	float m_fSearchDistanceShown = 3;
	float m_fShortestDistanceShown;
	vector m_vSearchPosShown;
	IEntity m_pNearestAEDShown;
	
	float m_fSearchDistancePerform = 3;
	float m_fShortestDistancePerform;
	vector m_vSearchPosPerform;
	IEntity m_pNearestAEDPerform;
	
	protected float m_fLastUpdateTime;
	protected const float m_fUpdateRate = 500;
	
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
			return (m_pNearestAEDShown && !AEDAlreadyFoundAndConnected());
		}
		
		// check for nearby AED
		QueryForAEDShown(patient.GetOrigin(), 3);
		if (!m_pNearestAEDShown)
		{
			Print("ACE_Medical_AEDConnectPatientUserAction::CanBeShownScript | No AED Found!", LogLevel.WARNING);
			return false;
		}
		
		// check for defib component
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(m_pNearestAEDShown.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		// check for connected patient
		if (defibrillatorComponent.GetConnectedPatient())
		{
			Print("ACE_Medical_AEDConnectPatientUserAction::CanBeShownScript | Patient already connected!", LogLevel.WARNING);
			return false;
		}
		
		Print("ACE_Medical_AEDConnectPatientUserAction::CanBeShownScript | AED found and ready for patient");

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		PrintFormat("ACE_MedicalDefibrillation_DefibrillationConnectPatientUserAction::Perform Action | Server Execution: %1", Replication.IsServer());
		
		// check for neabyAED
		QueryForAEDPerform(pOwnerEntity.GetOrigin(), 3);
		if (!m_pNearestAEDPerform)
		{
			Print("ACE_Medical_AEDConnectPatientUserAction::PerformAction | No Defibrillator Found!", LogLevel.WARNING);
			return;
		}
		
		// check for defib compponent
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(m_pNearestAEDPerform.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
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
		
		if (m_fLastUpdateTime + m_fUpdateRate > currentTime)
			return false;
		
		m_fLastUpdateTime = currentTime;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool AEDAlreadyFoundAndConnected()
	{
		if (!m_pNearestAEDShown)
			return false;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(m_pNearestAEDShown.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
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
	protected void QueryForAEDShown(vector searchPos, float searchDistance)
	{
		m_pNearestAEDShown = null;
		m_vSearchPosShown = searchPos;
		m_fSearchDistanceShown = searchDistance;
		
		m_fShortestDistanceShown = m_fSearchDistanceShown;
		
		GetGame().GetWorld().QueryEntitiesBySphere(m_vSearchPosShown, m_fSearchDistanceShown, QueryForAEDShownCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void QueryForAEDPerform(vector searchPos, float searchDistance)
	{
		m_pNearestAEDPerform = null;
		m_vSearchPosPerform = searchPos;
		m_fSearchDistancePerform = searchDistance;
		
		m_fShortestDistancePerform = m_fSearchDistancePerform;
		
		GetGame().GetWorld().QueryEntitiesBySphere(m_vSearchPosPerform, m_fSearchDistancePerform, QueryForAEDPerformCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool QueryForAEDShownCallback(IEntity entity)
	{
		if (!entity)
			return true;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return true;
		
		float distance = vector.Distance(m_vSearchPosShown, entity.GetOrigin());
		
		if (m_fShortestDistanceShown > distance)
		{
			m_fShortestDistanceShown = distance;
			m_pNearestAEDShown = entity;
		}
		
		return (m_fShortestDistanceShown != 0);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool QueryForAEDPerformCallback(IEntity entity)
	{
		if (!entity)
			return true;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return true;
		
		float distance = vector.Distance(m_vSearchPosPerform, entity.GetOrigin());
		
		if (m_fShortestDistancePerform > distance)
		{
			m_fShortestDistancePerform = distance;
			m_pNearestAEDPerform = entity;
		}
		
		return (m_fShortestDistancePerform != 0);
	}
}