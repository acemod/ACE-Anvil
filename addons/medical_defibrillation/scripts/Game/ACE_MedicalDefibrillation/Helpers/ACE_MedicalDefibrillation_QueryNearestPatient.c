class ACE_MedicalDefibrillation_QueryNearestPatient : ACE_QueryNearestEntity
{
	//------------------------------------------------------------------------------------------------
	//! Store entity if it is the closest
	override protected bool QueryCallback(IEntity entity)
	{
		super.QueryCallback(entity);
		
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(m_pNearestEntity.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitalsComponent)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return nearest entity
	override IEntity GetEntity(vector pos)
	{
		super.GetEntity(pos);
		
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(m_pNearestEntity.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitalsComponent)
			return null;
		
		return m_pNearestEntity;
	}
}