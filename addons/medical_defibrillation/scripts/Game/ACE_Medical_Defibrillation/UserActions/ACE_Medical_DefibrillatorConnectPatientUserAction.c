class ACE_Medical_DefibrillationConnectPatientUserAction: ScriptedUserAction
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
	
	protected bool m_bNameFound = false;
	
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
		IEntity patient = IEntity.Cast(GetOwner());
		if (!patient)
			return false;
		
		bool previousAEDFoundAndConnected = AEDAlreadyFoundAndConnected();
		// slow down tick rate
		if (!CanExecuteThisTick())
			return !previousAEDFoundAndConnected;
		
		// add name to action - once
		if (!m_bNameFound)
		{
			UIInfo uiInfo = GetUIInfo();
			string name = GetCharacterName(patient);
			if (!SCR_StringHelper.IsEmptyOrWhiteSpace(name))
			{
				uiInfo.SetName(string.Format("AED Connect Patient: %1", name));
				m_bNameFound = true;	
			}
		}
		
		// check for nearby AED
		QueryForAEDShown(patient.GetOrigin(), 3);
		if (!m_pNearestAEDShown)
		{
			Print("ACE_Medical_AEDConnectPatientUserAction::CanBeShownScript | No AED Found!", LogLevel.WARNING);
			return false;
		}
		
		// check for defib component
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(m_pNearestAEDShown.FindComponent(ACE_Medical_DefibrillatorComponent));
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
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		// server check
		if (!Replication.IsServer())
			return;
		
		// check for neabyAED
		QueryForAEDPerform(pOwnerEntity.GetOrigin(), 3);
		if (!m_pNearestAEDPerform)
		{
			Print("ACE_Medical_AEDConnectPatientUserAction::PerformAction | No AED Found!", LogLevel.WARNING);
			return;
		}
		
		// check for defib compponent
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(m_pNearestAEDPerform.FindComponent(ACE_Medical_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		defibrillatorComponent.ConnectPatient(pOwnerEntity);
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
		
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(m_pNearestAEDShown.FindComponent(ACE_Medical_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		if (!defibrillatorComponent.GetConnectedPatient())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetCharacterName(IEntity entity)
	{
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return "";
		
		string name;
		int id = playerManager.GetPlayerIdFromControlledEntity(entity);
		if (id != 0)
		{
			name = playerManager.GetPlayerName(id);
		}
		
		return name;
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
		
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(entity.FindComponent(ACE_Medical_DefibrillatorComponent));
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
		
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(entity.FindComponent(ACE_Medical_DefibrillatorComponent));
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