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
	override float ComputeReviveChance(ACE_Medical_CharacterContext context)
	{
		float result = super.ComputeReviveChance(context);
		
		if (s_pDefibSettings.m_bDefibIsMandatory && context.m_pVitals.GetShocksDelivered() == 0)
			result = 0;
		else
		{
			int shocks = context.m_pVitals.GetShocksDelivered();
			float maxBonus = 0.3;
			
			float bonusMultiplier = 1.0 - Math.Pow(0.5, shocks);
			float bonus = maxBonus * bonusMultiplier;
			
			result += bonus;
			result = Math.Min(result, 1.0);
		}
		
		PrintFormat("%1::ComputeReviveChance | Chance - %2", this.ClassName(), result);
		
		return result;
	}
}