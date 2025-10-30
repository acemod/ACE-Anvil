/*****
#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
modded class ArmaReforgerScripted : ChimeraGame
{
	//------------------------------------------------------------------------------------------------
	override bool OnGameStart()
	{
		DiagMenu.RegisterBool(SCR_DebugMenuID.ACE_MEDICAL_DEBUGUI_MENU_MEDICATION_DEBUG, "", "Debug medication", "ACE Medical");
		return super.OnGameStart();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(BaseWorld world, float timeslice)
	{
		super.OnUpdate(world, timeslice);
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.ACE_MEDICAL_DEBUGUI_MENU_MEDICATION_DEBUG))
			ACE_Medical_MedicationSystem.GetInstance(world).OnDiag(timeslice);
	}
}
#endif
*****/
