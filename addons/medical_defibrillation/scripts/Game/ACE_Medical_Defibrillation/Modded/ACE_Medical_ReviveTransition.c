modded class ACE_Medical_ReviveTransition
{
	protected static ACE_Medical_Defibrillation_Defibrillation_Settings s_pDefibSettings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_ReviveTransition(ACE_FSM_EStateID fromStateIDs, ACE_FSM_EStateID toStateID)
	{
		if (!s_pDefibSettings)
			s_pDefibSettings = ACE_SettingsHelperT<ACE_Medical_Defibrillation_Defibrillation_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeReviveChanceShockBonus(ACE_Medical_CharacterContext context)
	{
		float maxBonus = 0.3;
		ACE_Medical_Defibrillation_Defibrillation_Settings defibSettings = ACE_SettingsHelperT<ACE_Medical_Defibrillation_Defibrillation_Settings>.GetModSettings();
		if (defibSettings)
		{
			maxBonus = defibSettings.m_fMaxReviveChanceBonus;
		}
		
		int shocks = context.m_pVitals.GetShocksDelivered();
		
		float bonusMultiplier = 1.0 - Math.Pow(0.5, shocks);
		float bonus = maxBonus * bonusMultiplier;
		
		return bonus;
	}
	
	//------------------------------------------------------------------------------------------------
	override float ComputeReviveChance(ACE_Medical_CharacterContext context)
	{
		ACE_Medical_Defibrillation_Defibrillation_Settings defibSettings = ACE_SettingsHelperT<ACE_Medical_Defibrillation_Defibrillation_Settings>.GetModSettings();
		
		float result = super.ComputeReviveChance(context);
		
		bool isDefibMandatory = s_pDefibSettings.m_bDefibIsMandatory;
		int shocks = context.m_pVitals.GetShocksDelivered();
		
		if (isDefibMandatory && shocks == 0)
			result = 0;
		else
		{			
			result += ComputeReviveChanceShockBonus(context);
			result = Math.Min(result, 1.0);
		}
		
		return result;
	}
}