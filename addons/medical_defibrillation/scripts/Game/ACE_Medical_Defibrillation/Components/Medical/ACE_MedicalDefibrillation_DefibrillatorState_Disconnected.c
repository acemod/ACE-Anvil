class ACE_MedicalDefibrillation_DefibrillatorState_Disconnected : ACE_MedicalDefibrillation_DefibrillatorState_Base
{
	override void Enter()
	{
		m_DefibComponent.SetAnalysisPercent(0);
		m_DefibComponent.SetChargePercent(0);
		m_DefibComponent.ResetPatient();
		m_DefibComponent.SetCPRCooldown(m_DefibComponent.GetCPRCooldownTime());
		m_DefibComponent.SetInitialAnalysisPerformed(false);
	}
	override void Update(float timeSlice, float updateRate)
	{
		// If patient has been added, cycle to connected state
		if (m_DefibComponent.GetConnectedPatient())
		{
			PrintFormat("%1::Update | New patient detected. Changing state to connected.", this.ClassName());
			m_DefibComponent.ChangeState(new ACE_MedicalDefibrillation_DefibrillatorState_Connected(m_DefibEntity));
			return;
		}
	}
	override void Exit() {}
}