class ACE_MedicalDefibrillation_DefibrillatorState_Charged : ACE_MedicalDefibrillation_DefibrillatorState_Base
{
	override void Enter()
	{
		m_DefibComponent.PlaySound(m_DefibComponent.SOUNDCHARGED, true);
	}
	override void Update(float timeSlice, float updateRate)
	{
		// If no patient, go back to disconnected state
		if (!m_DefibComponent.GetConnectedPatient())
		{
			PrintFormat("%1::Update | No patient found. Changing back to disconnected state.", this.ClassName());
			m_DefibComponent.ChangeState(new ACE_MedicalDefibrillation_DefibrillatorState_Disconnected(m_DefibEntity));
			return;
		}
	}
	override void Exit() {}
}