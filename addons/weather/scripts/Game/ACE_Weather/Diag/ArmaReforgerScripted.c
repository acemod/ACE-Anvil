#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
modded class ArmaReforgerScripted : ChimeraGame
{
	protected bool m_bACE_Overheating_PreviousJamState = false;
	protected bool m_bACE_Overheating_PreviousJamSetting = false;
	protected float m_fACE_Overheating_PreviousBarrelTemperatureSetting;
	protected float m_fACE_Overheating_PreviousAmmoTemperatureSetting;
	
	//------------------------------------------------------------------------------------------------
	override bool OnGameStart()
	{
		DiagMenu.RegisterMenu(SCR_DebugMenuID.ACE_WEATHER_DEBUGUI_MENU, "ACE Weather", "ACE");
		DiagMenu.RegisterBool(SCR_DebugMenuID.ACE_WEATHER_DEBUGUI_MENU_TEMPERATURE_DIAG, "", "Show temperature diag", "ACE Weather");
		
		return super.OnGameStart();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(BaseWorld world, float timeslice)
	{
		super.OnUpdate(world, timeslice);
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.ACE_WEATHER_DEBUGUI_MENU_TEMPERATURE_DIAG))
			ACE_Weather_ShowTemperatureDiag()
	}
	

	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Weather_ShowTemperatureDiag()
	{
		ChimeraWorld world = GetGame().GetWorld();
		
		TimeAndWeatherManagerEntity manager = world.GetTimeAndWeatherManager();
		if (!manager)
			return;
		
		float outDoorTemperature = manager.ACE_GetCurrentOutdoorTemperature() - ACE_PhysicalConstants.ZERO_CELSIUS;
		
		DbgUI.Begin("ACE Weather Diag", 0, 700);
		DbgUI.Text(string.Format("Outdoor Air Temperature:      %1 deg C", ACE_Math.Round(outDoorTemperature, 1)));
		DbgUI.PlotLiveClamped("OutdoorTemperature", 500, 500, outDoorTemperature, -10, 40, 100, 10000);
		DbgUI.End();
	}
}
#endif