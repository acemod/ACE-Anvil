#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
modded class ArmaReforgerScripted : ChimeraGame
{
	//------------------------------------------------------------------------------------------------
	override bool OnGameStart()
	{
		DiagMenu.RegisterBool(SCR_DebugMenuID.ACE_MEDICAL_DEBUGUI_MENU_DEFIBRILLATOR_DEBUG, "", "Debug defibrillators", "ACE Medical");
		return super.OnGameStart();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(BaseWorld world, float timeslice)
	{
		super.OnUpdate(world, timeslice);
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.ACE_MEDICAL_DEBUGUI_MENU_DEFIBRILLATOR_DEBUG))
			ACE_Medical_Defibrillation_DefibStatesSystem.GetInstance(world).OnDiag(timeslice);
	}
}
#endif
