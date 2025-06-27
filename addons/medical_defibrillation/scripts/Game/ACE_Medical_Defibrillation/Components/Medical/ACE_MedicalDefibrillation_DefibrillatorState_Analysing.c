class ACE_MedicalDefibrillation_DefibrillatorState_Analysing : ACE_MedicalDefibrillation_DefibrillatorState_Base
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
		
		float analysisPercent = m_DefibComponent.GetAnalysisPercent();
		float analysisTime = m_DefibComponent.GetAnalysisTime();
		
		analysisPercent += updateRate / analysisTime / 1000;
		m_DefibComponent.SetAnalysisPercent(analysisPercent);
		
		// PrintFormat("%1::Update | Current Analysis Percent: %2", this.ClassName(), analysisPercent);
		
		if (analysisPercent >= 1.0)
		{
			m_DefibComponent.SetAnalysisPercent(1);
			m_DefibComponent.ChangeState(new ACE_MedicalDefibrillation_DefibrillatorState_Analysed(m_DefibEntity));
		}
		
		return;
	}
}