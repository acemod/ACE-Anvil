class ACE_MedicalDefibrillation_DefibrillatorState_Base
{
	IEntity m_DefibEntity;
	ACE_MedicalDefibrillation_DefibrillatorComponent m_DefibComponent;
	
	void ACE_MedicalDefibrillation_DefibrillatorState_Base(IEntity entity)
	{
		m_DefibEntity = entity;
		ACE_MedicalDefibrillation_DefibrillatorComponent defibComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibComponent)
		{
			// cause a problem
			delete this;
			return;
		}
		
		m_DefibComponent = defibComponent;
	}
	
	void Enter()
	{
		PrintFormat("%1::Enter | Not implimented", this.ClassName());
	}
	void Update(float timeSlice, float updateRate)
	{		
		PrintFormat("%1::Enter | Not implimented", this.ClassName());
	}
	void Exit()
	{
		PrintFormat("%1::Enter | Not implimented", this.ClassName());
	}
}