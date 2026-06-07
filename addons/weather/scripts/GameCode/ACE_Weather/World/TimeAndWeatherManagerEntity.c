//------------------------------------------------------------------------------------------------
//! Calculate air temperature based on corrected SinExp model
//! Ref.: Int. J. Climatol. 2006, 26, 75. (Equations 3a, 3b and 4)
modded class TimeAndWeatherManagerEntity : BaseTimeAndWeatherManagerEntity
{
	[Attribute(desc: "Average hour of the day at which peak air temperature occurs per month in 24 h format.", params: "0 24", category: "Temperature")]
	protected ref array<float> m_aACE_MonthlyAverageTemperatureAirMaxHours;
	
	[Attribute(desc: "Average daily minimum air temperature for each month in Kelvin", params: "0 1000", category: "Temperature")]
	protected ref array<float> m_aACE_MonthlyAverageDailyTemperatureAirMins;
	
	[Attribute(desc: "Average daily maximum air temperature for each month in Kelvin", params: "0 1000", category: "Temperature")]
	protected ref array<float> m_aACE_MonthlyAverageDailyTemperatureAirMaxs;
	
	[Attribute(defvalue: "1.45", desc: "Overcast-induced temperature shift is overcast times this factor in Kelvin. Symmetrically shifts Tmax down and Tmin up to lower DTR.", category: "Temperature")]
	protected float m_fACE_OvercastTemperatureFactor;
	
	[Attribute(defvalue: "2.2", desc: "Exponential temperature decay rate at night. γ in SinExp model.", params: "0 100", category: "Temperature")]
	protected float m_fACE_SinExp_Gamma;
	
	[Attribute(defvalue: "0.888", desc: "Autoregressive coefficient of mean temperature noise.", params: "0 1", category: "Temperature")]
	protected float m_fACE_MeanTemperatureAirAutoRegCoeff;
	
	[Attribute(defvalue: "3.218", desc: "Standard deviation of mean temperature noise in Kelvin.", params: "0 1000", category: "Temperature")]
	protected float m_fACE_MeanTemperatureAirStdDev;
	
	[Attribute(defvalue: "0.566", desc: "Autoregressive coefficient of diurnal temperature range noise.", params: "0 1", category: "Temperature")]
	protected float m_fACE_DiurnalTemperatureRangeAutoRegCoeff;
	
	[Attribute(defvalue: "1.204", desc: "Standard deviation of diurnal temperature range noise in Kelvin.", params: "0 1000", category: "Temperature")]
	protected float m_fACE_DiurnalTemperatureRangeStdDev;
	
	protected float m_fACE_CurrentAirTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE; // Default value to prevent instant freezing
	protected bool m_bACE_IsCurrentlyDay;

	protected float m_fACE_SinExp_Tmin;
	protected float m_fACE_SinExp_Tmax;
	protected float m_fACE_SinExp_Ts;
	protected float m_fACE_SinExp_Alpha;
	protected float m_fACE_SinExp_Tau;
	protected float m_fACE_SinExp_L;
	protected float m_fACE_SinExp_Hr;
	protected float m_fACE_SinExp_Hs;
	protected float m_fACE_SinExp_Hmax;
	
	protected ref ACE_AutoRegModel m_fACE_DiurnalTemperatureRangeNoiseGenerator;
	protected ref ACE_AutoRegModel m_fACE_MeanTemperatureAirNoiseGenerator;

	//------------------------------------------------------------------------------------------------
	//! TODO: Better approach for accuratly initializing SinExp from any starting point
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		if (!GetGame().InPlayMode())
			return;

		RplComponent rpl = SCR_EntityHelper.GetEntityRplComponent(owner);
		if (rpl && rpl.IsProxy())
			return;
		
		m_fACE_DiurnalTemperatureRangeNoiseGenerator = ACE_AutoRegModel(m_fACE_DiurnalTemperatureRangeAutoRegCoeff, m_fACE_DiurnalTemperatureRangeStdDev);
		m_fACE_MeanTemperatureAirNoiseGenerator = ACE_AutoRegModel(m_fACE_MeanTemperatureAirAutoRegCoeff, m_fACE_MeanTemperatureAirStdDev);
		
		// Day init always has to be done
		m_fACE_SinExp_Tmin = ACE_InterpolateForDayFromMonthlyAverage(GetMonth(), GetDay(), m_aACE_MonthlyAverageDailyTemperatureAirMins);
		m_fACE_SinExp_Tmin += m_fACE_OvercastTemperatureFactor * ACE_ComputeAverageOvercastForecast(12);
		m_fACE_SinExp_Tmin += m_fACE_DiurnalTemperatureRangeNoiseGenerator.SamplePoint();
		m_fACE_SinExp_Tmin += m_fACE_MeanTemperatureAirNoiseGenerator.SamplePoint();
		ACE_UpdateSunrisePortion(GetYear(), GetMonth(), GetDay());
		Print(m_fACE_SinExp_Hr);
		Print(m_fACE_SinExp_Hs);
		m_bACE_IsCurrentlyDay = IsDayHour(GetTimeOfTheDay());
		if (!m_bACE_IsCurrentlyDay)
		{
			m_fACE_CurrentAirTemperature = ACE_CalculateAirTemperature(m_fACE_SinExp_Hs - 0.001); // Get sunset temp slightly before sunset, will be loaded into sunset temp by updatesunsetportion
			ACE_UpdateSunsetPortion(GetYear(), GetMonth(), GetDay());
		}
	}

	//------------------------------------------------------------------------------------------------
	override void ACE_UpdateWeather(float timeSlice)
	{
		super.ACE_UpdateWeather(timeSlice);

		if (IsDayHour(GetTimeOfTheDay()) != m_bACE_IsCurrentlyDay)  // If out of date, update the params
		{
			m_bACE_IsCurrentlyDay = !m_bACE_IsCurrentlyDay;
			if (m_bACE_IsCurrentlyDay)
				ACE_UpdateSunrisePortion(GetYear(), GetMonth(), GetDay());
			else
				ACE_UpdateSunsetPortion(GetYear(), GetMonth(), GetDay());
		}

		m_fACE_CurrentAirTemperature = ACE_CalculateAirTemperature(GetTimeOfTheDay());
	}

	//------------------------------------------------------------------------------------------------
	float ACE_GetAirTemperature()
	{
		return m_fACE_CurrentAirTemperature;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ACE_CalculateAirTemperature(float currentTime)
	{
		if (currentTime < m_fACE_SinExp_Hr)  // Post midnight, pre sunrise
		{
			return m_fACE_SinExp_Tau + (m_fACE_SinExp_Ts - m_fACE_SinExp_Tau) * ACE_Math.Exp(-m_fACE_SinExp_Gamma * (currentTime + 24 - m_fACE_SinExp_Hs) / (24 - m_fACE_SinExp_L));
		}
		else if (currentTime < m_fACE_SinExp_Hs)  // sun is out, post sunrise pre sunset
		{
			float phase = Math.PI * (currentTime - m_fACE_SinExp_Hr) / (m_fACE_SinExp_L + 2 * m_fACE_SinExp_Alpha);
			return m_fACE_SinExp_Tmin + (m_fACE_SinExp_Tmax - m_fACE_SinExp_Tmin) * Math.Sin(phase);
		}
		else  // sun is set, pre midnight
		{
			return m_fACE_SinExp_Tau + (m_fACE_SinExp_Ts - m_fACE_SinExp_Tau) * ACE_Math.Exp(-m_fACE_SinExp_Gamma * (currentTime - m_fACE_SinExp_Hs) / (24 - m_fACE_SinExp_L));
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_UpdateSunrisePortion(int year, int month, int day)
	{
		m_fACE_SinExp_Hmax = ACE_InterpolateForDayFromMonthlyAverage(month, day, m_aACE_MonthlyAverageTemperatureAirMaxHours);
		m_fACE_SinExp_Tmax = ACE_InterpolateForDayFromMonthlyAverage(month, day, m_aACE_MonthlyAverageDailyTemperatureAirMaxs);
		m_fACE_SinExp_Tmax -= m_fACE_OvercastTemperatureFactor * ACE_ComputeAverageOvercastForecast(12);
		m_fACE_SinExp_Tmax -= m_fACE_DiurnalTemperatureRangeNoiseGenerator.SamplePoint();
		m_fACE_SinExp_Tmax += m_fACE_MeanTemperatureAirNoiseGenerator.SamplePoint();
		
		if (!GetSunriseHour(m_fACE_SinExp_Hr))
			m_fACE_SinExp_Hr = 0; // Workaround for polar circles
				
		if (!GetSunsetHour(m_fACE_SinExp_Hs))
			m_fACE_SinExp_Hs = 24; // Workaround for polar circles
				
		m_fACE_SinExp_L = m_fACE_SinExp_Hs - m_fACE_SinExp_Hr;
		m_fACE_SinExp_Alpha = m_fACE_SinExp_Hmax - (m_fACE_SinExp_Hr + m_fACE_SinExp_Hs) / 2;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_UpdateSunsetPortion(int year, int month, int day)
	{
		m_fACE_SinExp_Ts = m_fACE_CurrentAirTemperature;
		
		ACE_AddDaysToDate(year, month, day, 1);	 // Get tommorow's date
		
		float dstOffset = 0;
		if (IsDSTEnabled())
			dstOffset = GetDSTOffset();
		
		float hrPrime;
		if (!GetSunriseHourForDate(year, month, day, GetCurrentLatitude(), GetCurrentLongitude(), GetTimeZoneOffset(), dstOffset, hrPrime))
			hrPrime = 0; // Workaround for polar circles
				
		m_fACE_SinExp_Tmin = ACE_InterpolateForDayFromMonthlyAverage(month, day, m_aACE_MonthlyAverageDailyTemperatureAirMins);
		m_fACE_SinExp_Tmin += m_fACE_OvercastTemperatureFactor * ACE_ComputeAverageOvercastForecast(12);
		m_fACE_SinExp_Tmin += m_fACE_DiurnalTemperatureRangeNoiseGenerator.SamplePoint();
		m_fACE_SinExp_Tmin += m_fACE_MeanTemperatureAirNoiseGenerator.SamplePoint();
		float expResult = ACE_Math.Exp(-m_fACE_SinExp_Gamma * (24 + hrPrime - m_fACE_SinExp_Hs) / (24 - m_fACE_SinExp_L));
		m_fACE_SinExp_Tau = (m_fACE_SinExp_Tmin - m_fACE_SinExp_Ts * expResult) / (1 - expResult);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ACE_InterpolateForDayFromMonthlyAverage(int month, int day, array<float> monthlyAverages)
	{
		float lambda = (day - 0.5 * ACE_AVERAGE_DAYS_PER_MONTH) / ACE_AVERAGE_DAYS_PER_MONTH;
		
		if (lambda >= 0)
			return Math.Lerp(monthlyAverages[month - 1], monthlyAverages[month % 12], lambda);
		else
			return Math.Lerp(monthlyAverages[month - 1], monthlyAverages[(month - 2) % 12], -lambda);
	}
		
	//------------------------------------------------------------------------------------------------
	//! Compute average overcast for given duration in hours
	protected float ACE_ComputeAverageOvercastForecast(float forecastDuration)
	{
		float weight = 1.0;
		
		float changeDuration = GetTransitionManager().GetTimeLeftUntilNextState();
		if (changeDuration < forecastDuration)
			weight = GetTransitionManager().GetTimeLeftUntilNextState() / forecastDuration;
		
		float average = 0.0;
		average += weight * ACE_GetOvercastForState(GetTransitionManager().GetCurrentState());
		average += (1.0 - weight) * ACE_GetOvercastForState(GetTransitionManager().GetNextState());
		return average;
	}	
}
