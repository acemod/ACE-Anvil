class ACE_MedicalDefibrillation_DefibrillatorState_Connected : ACE_MedicalDefibrillation_DefibrillatorState_Base
{
	override void Enter()
	{
		m_DefibComponent.SetAnalysisPercent(0);
		m_DefibComponent.SetChargePercent(0);
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
		
		// If not an AED, do nothing
		if (m_DefibComponent.GetDefibrillationEmulation() != ACE_MedicalDefibrillation_EDefibrillatorEmulation.Automated)
			return;
		
		float CPRCooldown = m_DefibComponent.GetCPRCooldown();
		CPRCooldown -= updateRate / 1000;
		m_DefibComponent.SetCPRCooldown(CPRCooldown);
		
		if (!m_DefibComponent.GetInitialAnalysisPerformed())
		{
			m_DefibComponent.SetInitialAnalysisPerformed(true);
			CPRCooldown = 0;
			m_DefibComponent.SetCPRCooldown(CPRCooldown);
		}
		
		// PrintFormat("%1::Update | Current CPR Cooldown: %2", this.ClassName(), CPRCooldown);
		
		if (CPRCooldown <= 0)
		{
			// Change state to analysing
			PrintFormat("%1::Update | CPR cooldown complete. Changing to analysis state.", this.ClassName());
			m_DefibComponent.ChangeState(new ACE_MedicalDefibrillation_DefibrillatorState_Analysing(m_DefibEntity));
			return;
		}
	}
	
	override void Exit()
	{
		m_DefibComponent.SetCPRCooldown(m_DefibComponent.GetCPRCooldownTime());
	}
}