//------------------------------------------------------------------------------------------------
//! Calculate outdoor temperature based on corrected SinExp model
//! Ref.: Int. J. Climatol. 2006, 26, 75. (Equations 3a, 3b and 4)
modded class TimeAndWeatherManagerEntity : BaseTimeAndWeatherManagerEntity
{
	[Attribute(desc: "Average hour of the day at which peak air temperature occurs per month in 24 h format.", params: "0 24", category: "Temperature")]
	protected ref array<float> m_aACE_MonthlyAverageTemperatureAirMaxHours;
	
	[Attribute(desc: "Average daily minimum air temperature for each month in Kelvin", params: "0 1000", category: "Temperature")]
	protected ref array<float> m_aACE_MonthlyAverageDailyTemperatureAirMins;
	
	[Attribute(desc: "Average daily maximum air temperature for each month in Kelvin", params: "0 1000", category: "Temperature")]
	protected ref array<float> m_aACE_MonthlyAverageDailyTemperatureAirMaxs;
	
	[Attribute(defvalue: "2.2", desc: "Exponential temperature decay rate at night. γ in SinExp model.", params: "0 100", category: "Temperature")]
	protected float m_fACE_SinExp_Gamma;
	
	protected float m_fACE_CurrentOutdoorTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE; // Buffer value to prevent instant freezing
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
		
		// Day init always has to be done
		m_fACE_SinExp_Tmin = ACE_InterpolateForDayFromMonthlyAverage(GetMonth(), GetDay(), m_aACE_MonthlyAverageDailyTemperatureAirMins);
		ACE_UpdateSunrisePortion(GetYear(), GetMonth(), GetDay());
		Print(m_fACE_SinExp_Hr);
		Print(m_fACE_SinExp_Hs);
		m_bACE_IsCurrentlyDay = IsDayHour(GetTimeOfTheDay());
		if (!m_bACE_IsCurrentlyDay)
		{
			m_fACE_CurrentOutdoorTemperature = ACE_CalculateOutdoorTemperature(
				m_fACE_SinExp_Hs - 0.001);	 // Get sunset temp slightly before sunset, will be loaded into sunset temp by updatesunsetportion
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

		m_fACE_CurrentOutdoorTemperature = ACE_CalculateOutdoorTemperature(GetTimeOfTheDay());
	}

	//------------------------------------------------------------------------------------------------
	float ACE_GetCurrentOutdoorTemperature()
	{
		return m_fACE_CurrentOutdoorTemperature;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ACE_CalculateOutdoorTemperature(float currentTime)
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
		m_fACE_SinExp_Ts = m_fACE_CurrentOutdoorTemperature;
		
		ACE_AddDaysToDate(year, month, day, 1);	 // Get tommorow's date
		
		float dstOffset = 0;
		if (IsDSTEnabled())
			dstOffset = GetDSTOffset();
		
		float hrPrime;
		if (!GetSunriseHourForDate(year, month, day, GetCurrentLatitude(), GetCurrentLongitude(), GetTimeZoneOffset(), dstOffset, hrPrime))
			hrPrime = 0; // Workaround for polar circles
				
		m_fACE_SinExp_Tmin = ACE_InterpolateForDayFromMonthlyAverage(month, day, m_aACE_MonthlyAverageDailyTemperatureAirMins);
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
}
