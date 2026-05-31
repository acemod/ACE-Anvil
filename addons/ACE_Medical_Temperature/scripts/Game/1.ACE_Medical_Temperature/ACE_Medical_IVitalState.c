modded class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_CharacterContext>{
	protected static ACE_Medical_Temperature_Settings s_pTemperatureSettings;
	protected static float s_fDefaultCoreTemperature;
	protected static float s_fDailyLowTemperature;
	protected static float s_fDailyHighTemperature;
	protected static float s_fWindchillPower;
	protected static float s_fIndoorLeakage;
	protected static float s_fCoreTemperatureEfficacy;
	protected static ChimeraWorld s_World;
	protected static TimeAndWeatherManagerEntity s_TimeManager;//Try and make this static
	protected static float s_fSunriseTime;
	protected static float s_fSunsetTime;
	protected static IEntity s_player;
	protected static float s_fNaturalCoreHeating;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_IVitalState(ACE_FSM_EStateID id)
	{
		if (!s_pTemperatureSettings)
		{
			s_pTemperatureSettings = ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings();
			s_fDefaultCoreTemperature = s_pTemperatureSettings.m_fDefaultCoreTemperature;
			s_fDailyLowTemperature=s_pTemperatureSettings.m_fDailyLowTemperature;
			s_fDailyHighTemperature=s_pTemperatureSettings.m_fDailyHighTemperature;
			s_fWindchillPower=s_pTemperatureSettings.m_fWindchillPower;
			s_fIndoorLeakage=s_pTemperatureSettings.m_fIndoorLeakage;
			s_fCoreTemperatureEfficacy=s_pTemperatureSettings.m_fCoreTemperatureEfficacy;
			s_fNaturalCoreHeating=s_pTemperatureSettings.m_fNaturalCoreHeating;
		}
		if (!s_World){
			s_World=GetGame().GetWorld();
		}
		if (!s_TimeManager){
			s_TimeManager=s_World.GetTimeAndWeatherManager();
		}
	}
	
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		updateTemperature(context,timeSlice);
	}
	protected void updateTemperature(ACE_Medical_CharacterContext context, float timeSlice)
	{
		//---Day night cycle adjustment---//
		
		//Get time manager to output sunrise and sunset times to these variables
		s_TimeManager.GetSunriseHour(s_fSunriseTime);
		s_TimeManager.GetSunsetHour(s_fSunsetTime);
		float s_fDayNightCyclePower=0;//Default to zero, if it's zero it means the sun is set
		if (s_TimeManager.IsDayHour(s_TimeManager.GetTime().m_iHours)){//If it is day
			float s_fDayNightCycleProgress = Math.InverseLerp(s_fSunriseTime,s_fSunsetTime,s_TimeManager.GetTimeOfTheDay());//How far along in the sun's path is the sun currently?
			s_fDayNightCyclePower = -4 * Math.Pow(s_fDayNightCycleProgress-0.5,2)+1;//Convert the linear x variable into a power according to this parabola
		}
		//Map the progress in the day night cycle to the temperature ranges specified in the config
		float m_fFinalOutdoorTemperature = Math.Lerp(s_fDailyLowTemperature,s_fDailyHighTemperature,s_fDayNightCyclePower);
		
		//Get player
		SCR_ChimeraCharacter player=context.m_pObject;
		//If a player exists
		
		if(player){
			//---Altitude adjustment---//
			vector pos = player.GetOrigin();//Get the position of the player
			float m_fAltitude=pos[1];//Get the altitude
			m_fFinalOutdoorTemperature -= m_fAltitude*(6.5/1000); //Adjust temperature by -6.5c per KM above sea level - 
			
			//---Windchill adjustment---//
			float m_fWindchillIndex = Math.Pow(s_fWindchillPower*s_TimeManager.GetWindSpeed(),0.8);//Windchill temperature is on a curve, gets less effective with time
			float m_fIndoorSignal = context.m_signalsManager.GetSignalValue(22);
			float m_fVehicleSignal = context.m_signalsManager.GetSignalValue(24);
			float m_fWindchillFactor= Math.Lerp(m_fWindchillIndex,s_fIndoorLeakage,Math.Max(m_fIndoorSignal,m_fVehicleSignal));//At zero protection, have full windchill power, when indoors / in a vehicle scale down windchill to the config value
			m_fFinalOutdoorTemperature-=m_fWindchillFactor;
		}
		
		
		//Calculate how different the outdoor air is to core temperature
		float m_fOutdoorTemperatureDiff = m_fFinalOutdoorTemperature - context.m_pVitals.GetTemperature();
		
		//Reduce the impact of outside temperature by insulation
		float m_fOutdoorTemperatureFactor = m_fOutdoorTemperatureDiff*context.m_pVitals.m_fInsulationFactor;
		
		//Simulating core temperature natural heating. The amount of power it has scales down linearly with blood lost
		float m_fNaturalHeatingFactor= context.m_pBloodHitZone.GetHealthScaled()*s_fNaturalCoreHeating/1000;
		
		//Simulating core temperature ability to sweat/heat itself up naturally. The amount of power it has scales down linearly with blood lost
		//The power is increased by the setting in the config
		float m_fNaturalCoreFactor = context.m_pBloodHitZone.GetHealthScaled()*s_fCoreTemperatureEfficacy/1000;
		if (context.m_pVitals.GetTemperature()>307){//If core is too hot, natural system instead should cool off
			m_fNaturalCoreFactor*=-1;
		} 
		
		
		float m_fFinalChange = m_fOutdoorTemperatureFactor + m_fNaturalHeatingFactor+m_fNaturalCoreFactor;
		//Apply all the changes to core temperature, scaled for timescale
		context.m_pVitals.SetTemperature(context.m_pVitals.GetTemperature() + m_fFinalChange*timeSlice);
		
		//---Debug Printouts---//
		Print(context.m_pVitals.m_fCoreTemperature-273);
		Print(m_fFinalOutdoorTemperature-273);
		Print(m_fFinalChange);
		Print(m_fOutdoorTemperatureFactor);
		Print(m_fNaturalHeatingFactor);
		Print(m_fNaturalCoreFactor);
		
		
	}
}