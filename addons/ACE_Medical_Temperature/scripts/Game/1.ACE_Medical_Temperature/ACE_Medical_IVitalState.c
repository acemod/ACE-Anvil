modded class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_CharacterContext>{
	protected static ACE_Medical_Temperature_Settings s_pTemperatureSettings;

	protected static ChimeraWorld s_World;
	protected static TimeAndWeatherManagerEntity s_TimeManager;//Try and make this static
	protected static IEntity s_player;
	protected static float s_fSunriseTime;
	protected static float s_fSunsetTime;
	protected static float s_fCurrentTime;
	protected static float s_fDayNightCyclePower;
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_IVitalState(ACE_FSM_EStateID id)
	{
		if (!s_pTemperatureSettings)
		{
			s_pTemperatureSettings = ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings();
			s_fSunriseTime=0;
			s_fSunsetTime=0;
		}
		if (!s_World){
			s_World=GetGame().GetWorld();
		}
		if (!s_TimeManager){
			s_TimeManager=s_World.GetTimeAndWeatherManager();
			s_TimeManager.GetSunriseHour(s_fSunriseTime);
			s_TimeManager.GetSunsetHour(s_fSunsetTime);
		}
	}
	
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		updateWetness(context,timeSlice);
		updateTemperature(context,timeSlice);
	}
	
	protected void updateWetness(ACE_Medical_CharacterContext context, float timeSlice){
//		float m_fWaterDepth = context.m_pSignalsManager.GetSignalValue(28);//Index for WaterDepth, which returns how deep the character is in water in meters
//		float m_fWaterDiff = m_fWaterDepth - context.m_pVitals.m_fWetness;
//		float m_fCurrentHumidity = 0;//Humidity peaks at 6 am, then settles as the sun comes up before slowly rising
//		if (m_fWaterDiff>0)//If the character is in deeper water than their current wetness
//		{
//			//Make them wetter, up to a max of their current water depth, adjusted by timeslice plus a small correction factor - do not let them get more wet than 1
//			context.m_pVitals.m_fWetness +=  Math.Max(timeSlice*(m_fWaterDiff+0.05)/3,m_fWaterDiff);
//		} else {
//			context.m_pVitals.m_fWetness += Math.Max(timeSlice*(m_fWaterDiff-0.05)/50,m_fWaterDiff);
//		}
	}
	
	protected void updateTemperature(ACE_Medical_CharacterContext context, float timeSlice)
	{
		//---Day night cycle adjustment---//
		
		s_fDayNightCyclePower=0;//Default to zero, if it's zero it means the sun is set
		//Get time manager to output sunrise and sunset times to these variables
		//Try to run these calcs once per second, if possible
		if (s_TimeManager&&s_TimeManager.IsDayHour(s_TimeManager.GetTime().m_iHours))
		{
			float s_fDayNightCycleProgress = Math.InverseLerp(s_fSunriseTime,s_fSunsetTime,s_TimeManager.GetTimeOfTheDay());//How far along in the sun's path is the sun currently?
			s_fDayNightCyclePower = -4 * Math.Pow(s_fDayNightCycleProgress-0.5,2)+1;//Convert the linear x variable into a power according to this parabola
		}
		//Map the progress in the day night cycle to the temperature ranges specified in the config
		float m_fOutdoorTemperature = Math.Lerp(s_pTemperatureSettings.m_fOutdoorDailyLowTemperature,s_pTemperatureSettings.m_fOutdoorDailyHighTemperature,s_fDayNightCyclePower);
		
		float m_fIndoorTemperature = Math.Lerp(s_pTemperatureSettings.m_fIndoorDailyLowTemperature,s_pTemperatureSettings.m_fIndoorDailyHighTemperature,s_fDayNightCyclePower);
		
		float m_fVehicleTemperature = Math.Lerp(s_pTemperatureSettings.m_fVehicleDailyLowTemperature,s_pTemperatureSettings.m_fVehicleDailyHighTemperature,s_fDayNightCyclePower);
		
		//---Once per player calculations, if possible---//
		
		
		//Get player
		SCR_ChimeraCharacter player=context.m_pObject;
		//If a player exists
		Print(player.IsInVehicle());
		if(!player)
			return;
		
		float m_fFinalAmbientTemperature;
		float m_fIndoorSignal = context.m_pSignalsManager.GetSignalValue(22);//Value for IsIndoors
		float m_fVehicleSignal = context.m_pSignalsManager.GetSignalValue(24);//Value for if in a vehicle
		if (m_fIndoorSignal>0)//If the player is indoors
		{ //Lerp based on how indoors they are, 1 being fully indoors
			m_fFinalAmbientTemperature = Math.Lerp(m_fOutdoorTemperature,m_fIndoorTemperature,m_fIndoorSignal);
		} 
		else if (m_fVehicleSignal>0)
		{//If the player is inside a vehicle 
			m_fFinalAmbientTemperature = m_fVehicleTemperature;//Don't bother lerping, since vehicle signal is either 0 or 1
		} 
		else 
		{
			m_fFinalAmbientTemperature = m_fOutdoorTemperature;
		}
		
		//---Altitude adjustment---//
		vector pos = player.GetOrigin();//Get the position of the player
		float m_fAltitude=pos[1];//Get the altitude
		m_fFinalAmbientTemperature -= m_fAltitude*(6.5/1000); //Adjust temperature by -6.5c per KM above sea level - 
				
		
		//---Windchill adjustment---//
		float m_fWindchillIndex = Math.Pow(s_pTemperatureSettings.m_fWindchillPower*s_TimeManager.GetWindSpeed(),0.8);//Windchill temperature is on a curve, gets diminishing returns with speed
		float m_fWindchillFactor= Math.Lerp(m_fWindchillIndex,m_fWindchillIndex*0.5,Math.Max(m_fIndoorSignal,m_fVehicleSignal));//At zero protection, have full windchill power, when indoors / in a vehicle scale down windchill to half power
		m_fFinalAmbientTemperature-=m_fWindchillFactor;
		
		
		
		//Calculate how different the outdoor air is to core temperature
		float m_fAmbientTemperatureDiff = m_fFinalAmbientTemperature - context.m_pVitals.GetTemperature();
		
		//Reduce the impact of outside temperature by insulation for a final temperature factor
		float m_fAmbientTemperatureFactor = m_fAmbientTemperatureDiff*context.m_pVitals.m_fInsulationScore;
	
		
		//Simulating core temperature natural heating. The amount of power it has scales down linearly with blood lost
		float m_fNaturalHeatingFactor= context.m_pBloodHitZone.GetHealthScaled()*s_pTemperatureSettings.m_fNaturalCoreHeating/1000;
		
		//Simulating core temperature ability to sweat/heat itself up naturally. The amount of power it has scales down linearly with blood lost
		//The power is increased by the setting in the config
		float m_fNaturalCoreFactor = context.m_pBloodHitZone.GetHealthScaled()*s_pTemperatureSettings.m_fCoreHeatEfficacy/1000;
		if (context.m_pVitals.GetTemperature()>307){//If core is too hot, natural system instead should cool off
			m_fNaturalCoreFactor*=-1;
		} 
		
		//Heating of heat pack is the amount of heat packs * the heating from a heat pack
		float m_fHeatpackFactor = context.m_pVitals.m_iHeatPackCount*s_pTemperatureSettings.m_fHeatpackHeating/1000;
		
		float m_fFinalChange = m_fAmbientTemperatureFactor + m_fNaturalHeatingFactor + m_fNaturalCoreFactor + m_fHeatpackFactor;
		//Apply all the changes to core temperature, scaled for timescale
		context.m_pVitals.SetTemperature(context.m_pVitals.GetTemperature() + m_fFinalChange*timeSlice);
		
		//---Debug Printouts---//
//		Print(context.m_pVitals.m_iHeatPackCount);
		Print(context.m_pVitals.m_fCoreTemperature-273);
//		Print(m_fFinalAmbientTemperature-273);
//		Print(m_fFinalChange);
//		Print(m_fAmbientTemperatureFactor);
//		Print(m_fNaturalHeatingFactor);
//		Print(m_fNaturalCoreFactor);
//		Print(m_fHeatpackFactor);
		
	}
}