modded class TimeAndWeatherManagerEntity : BaseTimeAndWeatherManagerEntity
{
	[Attribute(defvalue: "2.2", desc: "Exponential temperature decay rate at night. Corresponds to γ in SinExp model.", params: "0 100", category: "Temperature")]
	protected float m_fACE_Temperature_ExpDecayRate;
	
	[Attribute(desc: "Average hour of the day at which peak air temperature occurs per month in 24 h format.", params: "0 24", category: "Temperature")]
	protected ref array<float> m_aACE_MonthlyAverageTemperatureAirMaxHours;
	
	[Attribute(desc: "Average daily minimum air temperature for each month in Kelvin", params: "0 1000", category: "Temperature")]
	protected ref array<float> m_aACE_MonthlyAverageDailyTemperatureAirMins;
	
	[Attribute(desc: "Average daily maximum air temperature for each month in Kelvin", params: "0 1000", category: "Temperature")]
	protected ref array<float> m_aACE_MonthlyAverageDailyTemperatureAirMaxs;
	
	// Model using https://discord.com/channels/976165959041679380/1509719021908398121/1512238279070716037
	protected float m_fACE_CurrentOutdoorTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE; // Buffer value to prevent instant freezing
	protected float m_fACE_UpdateInterval;		   // One update per x seconds
	protected float m_fACE_UpdateTimer;			   // [s]
	protected bool m_bCurrentlyDay;

	float m_fDailyTemperatureMinimum;
	float m_fDailyTemperatureMaximum;
	float m_fDailySunsetTemperature;

	float m_fAlpha;
	float m_fACE_Temperature_Tau;

	float m_fDayLength;
	float m_fSunriseHour;
	float m_fSunsetHour;
	float m_fPeakTemperatureHour;
	
	static const float AVERAGE_DAYS_PER_YEAR = 365.2425;
	static const float AVERAGE_DAYS_PER_MONTH = AVERAGE_DAYS_PER_YEAR / 12;

	//------------------------------------------------------------------------------------------------
	protected void TimeAndWeatherManagerEntity(IEntitySource src, IEntity parent)
	{
		src.Get("Update Frequency", m_fACE_UpdateInterval);
	}

	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		if (!GetGame().InPlayMode())
			return;

		RplComponent rpl = SCR_EntityHelper.GetEntityRplComponent(owner);
		if (rpl && rpl.IsProxy())
			return;
		
		// Day init always has to be done
		m_fDailyTemperatureMinimum = InterpolateForDayFromMonthlyAverage(GetMonth(), GetDay(), m_aACE_MonthlyAverageDailyTemperatureAirMins);
		ACE_UpdateSunrisePortion(GetYear(), GetMonth(), GetDay());
		Print(m_fSunriseHour);
		Print(m_fSunsetHour);
		float localTimeOfTheDay = ACE_ToLocalTimeOfTheDay(GetTimeOfTheDay());
		m_bCurrentlyDay = m_fSunriseHour < localTimeOfTheDay && localTimeOfTheDay < m_fSunsetHour;
		if (!m_bCurrentlyDay)
		{
			m_fACE_CurrentOutdoorTemperature = ACE_CalculateOutdoorTemperature(
				m_fSunsetHour - 0.001);	 // Get sunset temp slightly before sunset, will be loaded into sunset temp by updatesunsetportion
			ACE_UpdateSunsetPortion(GetYear(), GetMonth(), GetDay());
		}
	}

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);

		m_fACE_UpdateTimer -= timeSlice;
		if (m_fACE_UpdateTimer > 0)
			return;

		m_fACE_UpdateTimer = m_fACE_UpdateInterval;
		
		float localTimeOfTheDay = ACE_ToLocalTimeOfTheDay(GetTimeOfTheDay());

		if ((m_fSunriseHour < localTimeOfTheDay && localTimeOfTheDay < m_fSunsetHour) != m_bCurrentlyDay)  // If out of date, update the params
		{
			m_bCurrentlyDay = !m_bCurrentlyDay;
			if (m_bCurrentlyDay)
				ACE_UpdateSunrisePortion(GetYear(), GetMonth(), GetDay());
			else
				ACE_UpdateSunsetPortion(GetYear(), GetMonth(), GetDay());
		}

		m_fACE_CurrentOutdoorTemperature = ACE_CalculateOutdoorTemperature(localTimeOfTheDay);
		float timeStamp = GetTimeOfTheDay() + 24 * GetDay() - 24;
		Print(timeStamp);
		Print(m_fACE_CurrentOutdoorTemperature);
	}

	//------------------------------------------------------------------------------------------------
	float ACE_GetCurrentOutdoorTemperature()
	{
		return m_fACE_CurrentOutdoorTemperature;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ACE_CalculateOutdoorTemperature(float currentTime)
	{
		if (currentTime < m_fSunriseHour)  // Post midnight, pre sunrise
		{
			return m_fACE_Temperature_Tau + (m_fDailySunsetTemperature - m_fACE_Temperature_Tau) * ACE_Math.Exp(-m_fACE_Temperature_ExpDecayRate * (currentTime + 24 - m_fSunsetHour) / (24 - m_fDayLength));
		}
		else if (currentTime < m_fSunsetHour)  // sun is out, post sunrise pre sunset
		{
			float m_fInputBuffer = Math.PI * (currentTime - m_fSunriseHour) / (m_fDayLength + 2 * m_fAlpha);
			return m_fDailyTemperatureMinimum + (m_fDailyTemperatureMaximum - m_fDailyTemperatureMinimum) * Math.Sin(m_fInputBuffer);
		}
		else  // sun is set, pre midnight
		{
			return m_fACE_Temperature_Tau + (m_fDailySunsetTemperature - m_fACE_Temperature_Tau) * ACE_Math.Exp(-m_fACE_Temperature_ExpDecayRate * (currentTime - m_fSunsetHour) / (24 - m_fDayLength));
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_UpdateSunrisePortion(int year, int month, int day)
	{
		m_fPeakTemperatureHour = InterpolateForDayFromMonthlyAverage(month, day, m_aACE_MonthlyAverageTemperatureAirMaxHours);
		m_fDailyTemperatureMaximum = InterpolateForDayFromMonthlyAverage(month, day, m_aACE_MonthlyAverageDailyTemperatureAirMaxs);
		
		if (!GetSunriseHour(m_fSunriseHour))
			m_fSunriseHour = 0; // Workaround for polar circles
		
		m_fSunriseHour = ACE_ToLocalTimeOfTheDay(m_fSunriseHour);
		
		if (!GetSunsetHour(m_fSunsetHour))
			m_fSunsetHour = 24; // Workaround for polar circles
		
		m_fSunsetHour = ACE_ToLocalTimeOfTheDay(m_fSunsetHour);
		
		m_fDayLength = m_fSunsetHour - m_fSunriseHour;
		m_fAlpha = m_fPeakTemperatureHour - (m_fSunriseHour + m_fSunsetHour) / 2;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_UpdateSunsetPortion(int year, int month, int day)
	{
		m_fDailySunsetTemperature = m_fACE_CurrentOutdoorTemperature;
		
		ACE_AddDaysToDate(year, month, day, 1);	 // Get tommorow's date
		
		float sunriseHourPrime;
		if (!GetSunriseHourForDate(year, month, day, GetCurrentLatitude(), GetCurrentLongitude(), GetTimeZoneOffset(), GetDSTOffset(), sunriseHourPrime))
			sunriseHourPrime = 0; // Workaround for polar circles
		
		sunriseHourPrime = ACE_ToLocalTimeOfTheDay(sunriseHourPrime);
		
		m_fDailyTemperatureMinimum = InterpolateForDayFromMonthlyAverage(month, day, m_aACE_MonthlyAverageDailyTemperatureAirMins);
		float expResult = ACE_Math.Exp(-m_fACE_Temperature_ExpDecayRate * (24 + sunriseHourPrime - m_fSunsetHour) / (24 - m_fDayLength));
		m_fACE_Temperature_Tau = (m_fDailyTemperatureMinimum - m_fDailySunsetTemperature * expResult) / (1 - expResult);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float InterpolateForDayFromMonthlyAverage(int month, int day, array<float> monthlyAverages)
	{
		float lambda = (day - 0.5 * AVERAGE_DAYS_PER_MONTH) / AVERAGE_DAYS_PER_MONTH;
		
		if (lambda >= 0)
			return Math.Lerp(monthlyAverages[month - 1], monthlyAverages[month % 12], lambda);
		else
			return Math.Lerp(monthlyAverages[month - 1], monthlyAverages[(month - 2) % 12], -lambda);
	}
}
