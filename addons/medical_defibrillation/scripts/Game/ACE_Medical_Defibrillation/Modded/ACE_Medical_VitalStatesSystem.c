modded class ACE_Medical_VitalStatesSystem
{
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	//! Show vitals for target entity
	override void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		IEntity target;
		string targetType;
		if (!ACE_DiagTools.GetDiagTargetCharacter(target, targetType))
			return;
		
		ACE_Medical_VitalsComponent component = ACE_Medical_VitalsComponent.Cast(target.FindComponent(ACE_Medical_VitalsComponent));
		if (!component)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;
		
		DbgUI.Begin(string.Format("ACE Defibrillation: (%1)", targetType), 0, 500);
		DbgUI.Text(string.Format("Cardiac Rhythm:                      %1", SCR_Enum.GetEnumName(ACE_Medical_Defibrillation_ECardiacRhythm, component.GetCardiacRhythm())));
		DbgUI.Text(string.Format("Shocks Delivered:                    %1", component.GetShocksDelivered()));
		DbgUI.Text(string.Format("Revive Chance Bonus:                 %1", ACE_Medical_Defibrillation_DiagTools.GetReviveChanceShockBonus(target)));
		
		DbgUI.Spacer(10);
		
		float reviveChance = ACE_Medical_Defibrillation_DiagTools.GetReviveChance(target);
		DbgUI.Text(string.Format("Revive Chance:                       %1", reviveChance));
		DbgUI.PlotLiveClamped("Revive Chance", 500, 200, reviveChance, 0, 1, 100, 1000);
		
		DbgUI.End();
	}
#endif
}