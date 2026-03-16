//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_DefibContext : ACE_FrameJobScheduler_IObjectContext<IEntity>
{
	ACE_Medical_Defibrillation_DefibComponent m_pDefibrillator;
	
	void ACE_Medical_Defibrillation_DefibContext(IEntity object)
	{
		m_pDefibrillator = ACE_Medical_Defibrillation_DefibComponent.Cast(object.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool IsValid()
	{
		return (super.IsValid() && !m_pObject.IsDeleted());
	}
}