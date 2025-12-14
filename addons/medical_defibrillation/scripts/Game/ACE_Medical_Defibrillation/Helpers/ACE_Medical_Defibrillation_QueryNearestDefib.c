class ACE_Medical_Defibrillation_QueryNearestDefib : ACE_QueryNearestEntity
{
	//------------------------------------------------------------------------------------------------
	//! Pass search radius in ctor
	void ACE_Medical_Defibrillation_QueryNearestDefib(float searchDistance = 50)
	{
		m_fSearchDistanceM = searchDistance;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Store entity if it is the closest
	override protected bool QueryCallback(IEntity entity)
	{
		EntityPrefabData prefabData = entity.GetPrefabData();
		ResourceName prefabName = prefabData.GetPrefabName();
		
		ACE_Medical_Defibrillation_DefibComponent defibrillatorComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(entity.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibrillatorComponent)
			return true;
		
		float distance = vector.Distance(m_vSearchPos, entity.GetOrigin());
		
		if (m_fShortestDistanceM > distance)
		{
			m_fShortestDistanceM = distance;
			m_pNearestEntity = entity;
		}
		
		// Continue querying if distance didn't drop to zero
		return (m_fShortestDistanceM != 0);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return nearest entity
	override IEntity GetEntity(vector pos)
	{
		m_vSearchPos = pos;
		m_fShortestDistanceM = m_fSearchDistanceM;
		m_pNearestEntity = null;
		GetGame().GetWorld().QueryEntitiesBySphere(m_vSearchPos, m_fSearchDistanceM, QueryCallback);
		
		ACE_Medical_Defibrillation_DefibComponent defibrillatorComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(m_pNearestEntity.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibrillatorComponent)
			return null;
		
		EntityPrefabData prefabData = m_pNearestEntity.GetPrefabData();
		ResourceName prefabName = prefabData.GetPrefabName();
		PrintFormat("%1::QueryCallback | Defib found! %2", this.ClassName(), prefabName);
		return m_pNearestEntity;
	}
}