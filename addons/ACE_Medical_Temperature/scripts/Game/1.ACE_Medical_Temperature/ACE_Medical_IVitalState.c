modded class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_CharacterContext>{
	protected static ACE_Medical_Temperature_Settings s_pTemperatureSettings;
	protected static float s_fDefaultCoreTemperature;
	protected static float s_fDailyLowTemperature;
	protected static float s_fDailyHighTemperature;
	protected ChimeraWorld s_World;
	protected TimeAndWeatherManagerEntity s_TimeManager;//Try and make this static
	protected float s_fSunriseTime;
	protected float s_fSunsetTime;
	IEntity player = SCR_PlayerController.GetLocalMainEntity();
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_IVitalState(ACE_FSM_EStateID id)
	{
		if (!s_pTemperatureSettings)
		{
			s_pTemperatureSettings = ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings();
			s_fDefaultCoreTemperature = s_pTemperatureSettings.m_fDefaultCoreTemperature;
			s_fDailyLowTemperature=s_pTemperatureSettings.m_fDailyLowTemperature;
			s_fDailyHighTemperature=s_pTemperatureSettings.m_fDailyHighTemperature;
		}
	}
	
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		updateTemperature(context,timeSlice);
	}
	protected void updateTemperature(ACE_Medical_CharacterContext context, float timeSlice)
	{
		
//		//Get the brightness of the camera
//		float m_fBrightness=1-GetGame().GetWorld().GetCameraHDRBrightness(0);//This is awful. It changes depending on where you look. best i got
//		//Scores from ~1 at peak brighness to 0.6 at heavy shade
//		print(m_fBrightness);
		
		//This time manager should be in a static class - doesn't need updating by everyone. Get the time and weather manager
		s_World=GetGame().GetWorld();
		s_TimeManager=s_World.GetTimeAndWeatherManager();

		//Get time manager to output sunrise and sunset times to these variables
		s_TimeManager.GetSunriseHour(s_fSunriseTime);
		s_TimeManager.GetSunsetHour(s_fSunsetTime);
		float s_fDayNightCyclePower=0;//Default to zero, if it's zero it means the sun is set
		if (s_TimeManager.IsDayHour(s_TimeManager.GetTime().m_iHours)){//If it is day
			float s_fDayNightCycleProgress = Math.Map(s_TimeManager.GetTimeOfTheDay(),s_fSunriseTime,s_fSunsetTime,0,1);//How far along in the sun's path is the sun currently?
			s_fDayNightCyclePower = -4 * Math.Pow(s_fDayNightCycleProgress-0.5,2)+1;//Adjust the imapct of the sum according to this parabola
		}
		
		float m_fFinalOutdoorTemperature = Math.Map(s_fDayNightCyclePower,0,1,s_fDailyLowTemperature,s_fDailyHighTemperature);
		//Get the player
		if(player){
			vector pos = player.GetOrigin();//Get the position of the player
			float m_fAltitude=pos[1];//Get the altitude
			m_fFinalOutdoorTemperature -= m_fAltitude*(6.5/1000); //Adjust temperature by -6.5c per KM above sea level - 
		}
			
		
		
		
		//Get the current core temperature
		float m_fCurrentCoreTemperature=context.m_pVitals.m_fCoreTemperature;
		//Calculate how different the outdoor air is to core temperature
		float m_fOutdoorTemperatureDiff = m_fFinalOutdoorTemperature - context.m_pVitals.GetTemperature();
		
		//Reduce the impact of outside temperature by insulation
		float m_fCoreTemperatureChange = m_fOutdoorTemperatureDiff*context.m_pVitals.m_fInsulationFactor;
		
		//Apply all the changes to core temperature
		context.m_pVitals.SetTemperature(m_fCurrentCoreTemperature + m_fCoreTemperatureChange*timeSlice);
		
		//Todo: implement body heat system
		
		
	}
}