//------------------------------------------------------------------------------------------------
class ACE_MedicalDefibrillation_DefibrillatorContext : ACE_FrameJobScheduler_IObjectContext<IEntity>
{
	ACE_MedicalDefibrillation_DefibrillatorComponent m_pDefibrillator;
	
	void ACE_MedicalDefibrillation_DefibrillatorContext(IEntity object)
	{
		m_pDefibrillator = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(object.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool IsValid()
	{
		return (super.IsValid() && !m_pObject.IsDeleted());
	}
}