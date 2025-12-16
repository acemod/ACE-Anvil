//------------------------------------------------------------------------------------------------
class ACE_Medical_CharacterContext : ACE_FrameJobScheduler_IObjectContext<SCR_ChimeraCharacter>
{
	ACE_Medical_VitalsComponent m_pVitals;
	ACE_Medical_MedicationComponent m_pMedication;
	SCR_CharacterDamageManagerComponent m_pDamageManager;
	SCR_CharacterBloodHitZone m_pBloodHitZone;
	float m_fCPRSuccessCheckTimerS = 0;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_CharacterContext(SCR_ChimeraCharacter object)
	{
		m_pVitals = ACE_Medical_VitalsComponent.Cast(object.FindComponent(ACE_Medical_VitalsComponent));
		m_pMedication = ACE_Medical_MedicationComponent.Cast(object.FindComponent(ACE_Medical_MedicationComponent));
		m_pDamageManager = SCR_CharacterDamageManagerComponent.Cast(object.GetDamageManager());
		m_pBloodHitZone = m_pDamageManager.GetBloodHitZone();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool IsValid()
	{
		return (super.IsValid() && !m_pObject.IsDeleted());
	}
}
