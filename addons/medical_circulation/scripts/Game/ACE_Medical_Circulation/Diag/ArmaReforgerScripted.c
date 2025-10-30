/*****
#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
modded class ArmaReforgerScripted : ChimeraGame
{
	//------------------------------------------------------------------------------------------------
	override bool OnGameStart()
	{
		DiagMenu.RegisterBool(SCR_DebugMenuID.ACE_MEDICAL_DEBUGUI_MENU_VITAL_STATE_DEBUG, "", "Debug vital state", "ACE Medical");
		return super.OnGameStart();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(BaseWorld world, float timeslice)
	{
		super.OnUpdate(world, timeslice);
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.ACE_MEDICAL_DEBUGUI_MENU_VITAL_STATE_DEBUG))
			ACE_Medical_VitalStatesSystem.GetInstance().OnDiag(timeslice);
	}
}
#endif
*****/