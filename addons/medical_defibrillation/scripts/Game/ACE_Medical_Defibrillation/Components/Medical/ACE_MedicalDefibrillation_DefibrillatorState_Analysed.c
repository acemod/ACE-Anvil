class ACE_MedicalDefibrillation_DefibrillatorState_Analysed : ACE_MedicalDefibrillation_DefibrillatorState_Base
{
	override void Update(float timeSlice, float updateRate)
	{
		// If no patient, go back to disconnected state
		if (!m_DefibComponent.GetConnectedPatient())
		{
			PrintFormat("%1::Update | No patient found. Changing back to disconnected state.", this.ClassName());
			m_DefibComponent.ChangeState(new ACE_MedicalDefibrillation_DefibrillatorState_Disconnected(m_DefibEntity));
			return;
		}
		
		if (m_DefibComponent.IsShockableRhythm())
		{
			PrintFormat("%1::Update | Shockable rhythm found. Changing to charging state.", this.ClassName());
			m_DefibComponent.PlaySound(m_DefibComponent.SOUNDSHOCKADVISED);
			m_DefibComponent.PlaySound(m_DefibComponent.SOUNDCHARGING);
			m_DefibComponent.ChangeState(new ACE_MedicalDefibrillation_DefibrillatorState_Charging(m_DefibEntity));
		}
		else
		{
			// return to connected state
			PrintFormat("%1::Update | Not shockable rhythm found. Changing to connected state.", this.ClassName());
			m_DefibComponent.PlaySound(m_DefibComponent.SOUNDNOSHOCKADVISED);
			m_DefibComponent.ChangeState(new ACE_MedicalDefibrillation_DefibrillatorState_Connected(m_DefibEntity));
		}
	}
}