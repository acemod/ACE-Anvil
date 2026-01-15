#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
modded class ArmaReforgerScripted : ChimeraGame
{
	//------------------------------------------------------------------------------------------------
	override bool OnGameStart()
	{
		DiagMenu.RegisterMenu(SCR_DebugMenuID.ACE_DEBUGUI_MENU, "ACE", "");
		return super.OnGameStart();
	}
}
#endif
