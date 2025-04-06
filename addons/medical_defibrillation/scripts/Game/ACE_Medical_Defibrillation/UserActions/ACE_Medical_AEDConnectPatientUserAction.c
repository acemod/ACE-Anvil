class ACE_Medical_AEDConnectPatientUserAction: ScriptedUserAction
{
	protected const float m_fSearchDistance = 5;
	protected float m_fShortestDistance;
	protected vector m_vSearchPos;
	
	protected IEntity m_pNearestAED;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		UIInfo uiInfo = GetUIInfo();
		uiInfo.SetName(string.Format("AED - Connect Patient: %1", "Insert Name Here"));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		IEntity patient = IEntity.Cast(this.GetOwner());
		if (!patient)
			return false;
		
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
		
		// Bug here - exception if the patient is dead dead - how to fix?
		ResourceName prefab = entity.GetPrefabData().GetPrefabName();
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