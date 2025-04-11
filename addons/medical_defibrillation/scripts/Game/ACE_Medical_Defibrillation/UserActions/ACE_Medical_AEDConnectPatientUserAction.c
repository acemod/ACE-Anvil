class ACE_Medical_AEDConnectPatientUserAction: ScriptedUserAction
{
	protected const float m_fSearchDistance = 5;
	protected float m_fShortestDistance;
	protected vector m_vSearchPos;
	
	protected float m_fLastUpdateTime;
	protected const float m_fUpdateRate = 500;
	
	protected bool m_bNameFound = false;
	
	protected IEntity m_pNearestAED;
	
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
		IEntity patient = IEntity.Cast(this.GetOwner());
		if (!patient)
			return false;
		
		// slow down tick rate
		float currentTime = GetGame().GetWorld().GetWorldTime();
		if (m_fLastUpdateTime + m_fUpdateRate > currentTime)
		{
			if (m_pNearestAED)
			{
				ACE_Medical_AEDComponent component = ACE_Medical_AEDComponent.Cast(m_pNearestAED.FindComponent(ACE_Medical_AEDComponent));
				if (component)
				{
					if (component.GetConnectedPatient())
						return false;
				}
			}
			
			return m_pNearestAED != null;
		}
		
		m_fLastUpdateTime = currentTime;
		
		PrintFormat("Checking for near AED (time: %1)", currentTime);
		
		m_vSearchPos = patient.GetOrigin();
		
		// Query to find the nearest AED
		m_pNearestAED = null;
		m_fShortestDistance = m_fSearchDistance;
		GetGame().GetWorld().QueryEntitiesBySphere(m_vSearchPos, m_fSearchDistance, Callback);
		
		if (!m_pNearestAED)
		{
			Print("No near AED found!");
			return false;
		}
		
		// Add name to action - once
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
		
		ACE_Medical_AEDComponent component = ACE_Medical_AEDComponent.Cast(m_pNearestAED.FindComponent(ACE_Medical_AEDComponent));
		if (component)
		{
			if (component.GetConnectedPatient())
				return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		if (!m_pNearestAED)
			return;
		
		ACE_Medical_AEDComponent component = ACE_Medical_AEDComponent.Cast(m_pNearestAED.FindComponent(ACE_Medical_AEDComponent));
		if (!component)
			return;
		
		component.ConnectPatient(pOwnerEntity);
		component.PlaySound(ACE_Medical_AEDComponent.SOUNDCONNECTED, true);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool Callback(IEntity entity)
	{
		if (!entity)
			return true;
		
		EntityPrefabData prefabData = entity.GetPrefabData();
		if (!prefabData)
			return true;
		
		ResourceName prefab = prefabData.GetPrefabName();
		if (!(prefab == "{7E096143E0A4363E}Prefabs/Items/Medical Defibrillation/AED_Base.et"))
			return true;
		
		float distance = vector.Distance(m_vSearchPos, entity.GetOrigin());
		
		if (m_fShortestDistance > distance)
		{
			m_fShortestDistance = distance;
			m_pNearestAED = entity;
		}
		
		return (m_fShortestDistance != 0);
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
}