modded class ACE_Medical_CardiovascularSystem
{
	
	//------------------------------------------------------------------------------------------------
	override protected void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);

		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(entity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;
		
		UpdateCardiacRhythm(component, damageManager, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCardiacRhythm(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		ACE_Medical_EVitalState vitalState = component.GetVitalState();
		
		if (component.IsInCardiacArrest())
		{
			component.SetCardiacRhythm(ACE_Medical_ECardiacRhythm.VF);
			// TODO: impliment system for deterioration to PEA - we need two cardiac arrest states?
		}
		else
		{
			component.SetCardiacRhythm(ACE_Medical_ECardiacRhythm.Sinus);
		}
	}

#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override protected void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		IEntity target;
		string targetType;
		if (!GetDiagTarget(target, targetType))
			return;
		
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;

		ACE_Medical_ECardiacRhythm rhythm = component.GetCardiacRhythm();
		string rhythmName = SCR_Enum.GetEnumName(ACE_Medical_ECardiacRhythm, rhythm);
		
		DbgUI.Begin(string.Format("ACE_Medical_DefibrillationSystem (%1)", targetType), 0, 700);
		DbgUI.Text(string.Format("Cardiac rhythm:                  %1", rhythmName));
		DbgUI.Text(string.Format("# of Shocks:                  %1", component.GetShocksDelivered()));
		DbgUI.End();
	}
#endif
}