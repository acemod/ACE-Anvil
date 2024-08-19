//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationAdjustments : Managed
{
	float m_fHeartRate = 0;
	float m_fSystemicVascularResistence = 0;
	float m_fReviveSuccessCheckTimeout = 0;
	float m_fBloodFlowScale = 0;
	float m_fPain = 0;
	
	//------------------------------------------------------------------------------------------------
	void Apply(IEntity target)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (cardiovascularComponent)
		{
			cardiovascularComponent.SetHeartRateMedicationAdjustment(m_fHeartRate);
			cardiovascularComponent.SetSystemicVascularResistenceMedicationAdjustment(m_fSystemicVascularResistence);
		}
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager)
		{
			damageManager.ACE_Medical_SetBloodFlowScale(m_fBloodFlowScale);
			damageManager.ACE_Medical_SetPainSuppression(m_fPain);
		}
	}
}
