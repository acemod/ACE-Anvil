class ACE_MedicalDefibrillation_DefibrillatorState_Charging : ACE_MedicalDefibrillation_DefibrillatorState_Base
{
	override void Enter()
	{
		m_DefibComponent.PlaySound(m_DefibComponent.SOUNDCHARGING);
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
		
		float chargePercent = m_DefibComponent.GetChargePercent();
		float chargeTime = m_DefibComponent.GetChargeTime();
		
		chargePercent += updateRate / chargeTime / 1000;
		m_DefibComponent.SetChargePercent(chargePercent);
		
		if (chargePercent >= 1)
		{
			m_DefibComponent.SetChargePercent(1);
			m_DefibComponent.ChangeState(new ACE_MedicalDefibrillation_DefibrillatorState_Charged(m_DefibEntity));
		}
		
		return;
	}
}