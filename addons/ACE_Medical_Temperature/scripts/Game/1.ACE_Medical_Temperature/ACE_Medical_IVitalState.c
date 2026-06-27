modded class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_CharacterContext>{
	protected static ACE_Medical_Temperature_Settings s_pTemperatureSettings;

	protected static ChimeraWorld s_World;
	protected static TimeAndWeatherManagerEntity s_TimeManager;//Try and make this static
	protected static IEntity s_player;
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_IVitalState(ACE_FSM_EStateID id)
	{
		if (!s_pTemperatureSettings)
		{
			s_pTemperatureSettings = ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings();
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
		updateWetness(context,timeSlice);
		updateTemperature(context,timeSlice);
	}
	
	protected void updateWetness(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float m_fWaterDepth = Math.Min(Math.Pow(context.m_pSignalsManager.GetSignalValue(28),4)*0.8,1);//Index for WaterDepth, which returns how deep the character is in water in meters, squared because it's wonky, reduced a bit in impact, capped to 1
		float m_fWaterDiff = m_fWaterDepth - context.m_pVitals.m_fWetness;
		Print(m_fWaterDepth);
		if (m_fWaterDiff==0)
		{
			return; //Skip if no calculations need to be performed
		}
		else if (m_fWaterDiff>0)//If the character is in deeper water than their current wetness
		{
			//Make them wetter, up to a max of their current water depth, adjusted by timeslice plus a small correction factor - do not let them get more wet than 1
			context.m_pVitals.m_fWetness +=  Math.Min(timeSlice*(m_fWaterDiff/3+0.01),m_fWaterDiff);
		} else {
			//More wind = more drying
			float m_fDryingRate = 180;
			m_fDryingRate -= s_TimeManager.GetWindSpeed()*15;
			m_fDryingRate -= Math.Pow(s_TimeManager.ACE_GetAirTemperature()/250,2)*50;//Random bs number, more drying with more heat
			
			context.m_pVitals.m_fWetness += Math.Max(timeSlice*(m_fWaterDiff-0.01)/50,m_fWaterDiff);
		}
	}
	
	protected void updateTemperature(ACE_Medical_CharacterContext context, float timeSlice)
	{
		SCR_ChimeraCharacter player=context.m_pObject;
		if(!player)
			return;
		
		//Using air temp that kex wrote :), run pref once per update cycle
		float m_fOutdoorTemperature = s_TimeManager.ACE_GetAirTemperature(player.GetOrigin()[1]);
		
		float m_fIndoorTemperature = (m_fOutdoorTemperature-298.15)*s_pTemperatureSettings.m_fIndoorInsulation + 298.15;//travel insulation degrees for each degree that outdoor temp changes
		
		float m_fVehicleTemperature = (m_fOutdoorTemperature-298.15)*s_pTemperatureSettings.m_fVehicleInsulation + 298.15;
		
		//---Once per player calculations, if possible---//
		
		
		float m_fFinalAmbientTemperature;
		float m_fIndoorSignal = context.m_pSignalsManager.GetSignalValue(22);//Value for IsIndoors
		float m_fVehicleSignal = context.m_pSignalsManager.GetSignalValue(24);//Value for if in a vehicle
		
		if (m_fIndoorSignal>0)//Lerp between 0,1
			m_fFinalAmbientTemperature = Math.Lerp(m_fOutdoorTemperature,m_fIndoorTemperature,m_fIndoorSignal);
		else if (m_fVehicleSignal>0)
			m_fFinalAmbientTemperature = m_fVehicleTemperature;//Don't bother lerping, since vehicle signal is either 0 or 1
		else 
			m_fFinalAmbientTemperature = m_fOutdoorTemperature;
		
		//---Windchill adjustment---//
		float m_fWindchillIndex = Math.Pow(s_pTemperatureSettings.m_fWindchillPower*s_TimeManager.GetWindSpeed(),0.8);//Windchill temperature is on a curve, gets diminishing returns with speed
		float m_fWindchillFactor= Math.Lerp(m_fWindchillIndex,m_fWindchillIndex*0.3,Math.Max(m_fIndoorSignal,m_fVehicleSignal));//At zero protection, have full windchill power, when indoors / in a vehicle scale down windchill to 20% ish power
		m_fFinalAmbientTemperature-=m_fWindchillFactor;
		
		//Calculate how different the outdoor air is to core temperature
		float m_fAmbientTemperatureDiff = m_fFinalAmbientTemperature - context.m_pVitals.GetTemperature();
		
		//Reduce the impact of outside temperature by insulation, plus increaase the power of weather with more wet
		float m_fAmbientTemperatureFactor = m_fAmbientTemperatureDiff*context.m_pVitals.m_fInsulationScore* Math.Lerp(1,1.15,context.m_pVitals.m_fWetness);//At max wetness, 20 ish% more heat seeps out;
		
		//Simulating core temperature natural heating. The amount of power it has scales down linearly with blood lost
		float m_fNaturalHeatingFactor= context.m_pBloodHitZone.GetHealthScaled()*s_pTemperatureSettings.m_fNaturalCoreHeating/1000;
		
		//Simulating core temperature ability to sweat/heat itself up naturally. The amount of power it has scales down linearly with blood lost
		//The power is increased by the setting in the config
		float m_fNaturalCoreFactor = context.m_pBloodHitZone.GetHealthScaled()*s_pTemperatureSettings.m_fCoreHeatEfficacy/1000;
		if (context.m_pVitals.GetTemperature()>310.15){//If core is too hot, natural system instead should cool off
			m_fNaturalCoreFactor*=-1;
		} 
		
		//Heating of heat pack is the amount of heat packs * the heating from a heat pack
		float m_fHeatpackFactor = context.m_pVitals.m_iHeatPackCount*s_pTemperatureSettings.m_fHeatpackHeating/1000;
		
		float m_fFinalChange = m_fAmbientTemperatureFactor + m_fNaturalHeatingFactor + m_fNaturalCoreFactor + m_fHeatpackFactor;
		//Apply all the changes to core temperature, scaled for timescale
		context.m_pVitals.SetTemperature(context.m_pVitals.GetTemperature() + m_fFinalChange*timeSlice);
		
		//---Debug Printouts---//
		Print("--------Temperature--------");
		Print(context.m_pVitals.GetTemperature()-ACE_PhysicalConstants.ZERO_CELSIUS);
		Print(m_fFinalAmbientTemperature-ACE_PhysicalConstants.ZERO_CELSIUS);
		Print("--------Breakdown--------");
		Print(m_fFinalChange);
		Print(m_fAmbientTemperatureFactor);
		Print(m_fNaturalHeatingFactor);
		Print(m_fNaturalCoreFactor);
		Print(m_fHeatpackFactor);
		Print("--------Debug--------");
		Print(context.m_pVitals.m_iHeatPackCount);
		Print(context.m_pVitals.m_fWetness);
		Print(context.m_pVitals.m_fInsulationScore* Math.Lerp(1,1.15,context.m_pVitals.m_fWetness));
		
	}
}