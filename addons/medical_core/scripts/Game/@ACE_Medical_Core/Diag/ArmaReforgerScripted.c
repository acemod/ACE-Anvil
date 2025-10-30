#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
modded class ArmaReforgerScripted : ChimeraGame
{
	//------------------------------------------------------------------------------------------------
	override bool OnGameStart()
	{
		DiagMenu.RegisterMenu(SCR_DebugMenuID.ACE_MEDICAL_DEBUGUI_MENU, "ACE Medical", "ACE");
		return super.OnGameStart();
	}
}
#endif
