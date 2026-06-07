modded class TimeAndWeatherManagerEntity : BaseTimeAndWeatherManagerEntity 
{
	//Model using https://discord.com/channels/976165959041679380/1509719021908398121/1512238279070716037
	float m_fACE_CurrentOutdoorTemperature=293;//Buffer value to prevent instant freezing
	float m_fACE_UpdateInterval; // One update per x seconds
	float m_fACE_UpdateTimer; // [s]
	bool m_bCurrentlyDay; 
	bool m_bInitialized;
	
	
	float m_fDailyTemperatureMinimum;
	float m_fDailyTemperatureMaximum;
	float m_fDailySunsetTemperature;
	
	float m_fAlpha;
	float m_fExpResultPrime;//Used as a helper for calculating tau
	float m_fTau;
	
	float m_fDayLength;
	float m_fSunriseHour;
	float m_fSunsetHour;
	float m_fPeakTemperatureHour;
	float m_fSunriseHourPrime;
	
	[Attribute(defvalue: "2.2", desc:"Exponential decay function, symbolized as y in the equation", params: "0 100",category:"Temperature")]
	float m_fExpDecay;
	[Attribute(desc: "Hour at which peak temperature occurs, 24hr format", params: "0 24", category:"Temperature")]
	protected ref array<float> m_fMonthlyPeakTemperatureHour;
	[Attribute(desc: "Daily low temp for the 1st of each month, kelvin", params: "0 1000", category:"Temperature")]
	protected ref array<float> m_fMonthlyDailyLowTemperature;
	[Attribute(desc: "Daily high temp for the 1st of each month, kelvin", params: "0 1000", category:"Temperature")]
	protected ref array<float> m_fMonthlyDailyHighTemperature;
		
	//------------------------------------------------------------------------------------------------
	protected void TimeAndWeatherManagerEntity(IEntitySource src, IEntity parent)
	{
		src.Get("Update Frequency", m_fACE_UpdateInterval);
	}
	
	//------------------------------------------------------------------------------------------------
	void Init()
	{
		//Day init always has to be done
		m_fDailyTemperatureMinimum = Math.Lerp(m_fMonthlyDailyLowTemperature[GetMonth()-1],m_fMonthlyDailyLowTemperature[GetMonth()%12],(GetDay()-0.999999)/31);
		UpdateSunrisePortion(GetYear(),GetMonth(),GetDay());
		Print(m_fSunriseHour);
		Print(m_fSunsetHour);
		m_bCurrentlyDay = m_fSunriseHour<GetTimeOfTheDay() && GetTimeOfTheDay()<m_fSunsetHour;
		if (!m_bCurrentlyDay)
		{
			m_fACE_CurrentOutdoorTemperature = CalculateOutdoorTemperature(m_fSunsetHour-0.001); //Get sunset temp slightly before sunset, will be loaded into sunset temp by updatesunsetportion
			UpdateSunsetPortion(GetYear(),GetMonth(),GetDay());
		}
		
		
		m_bInitialized=true;
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
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		m_fACE_UpdateTimer -= timeSlice;
		if (m_fACE_UpdateTimer > 0)
			return;
		
		m_fACE_UpdateTimer = m_fACE_UpdateInterval;

		if (!m_bInitialized)
			Init();
		
		if ((m_fSunriseHour<GetTimeOfTheDay() && GetTimeOfTheDay()<m_fSunsetHour) != m_bCurrentlyDay)//If out of date, update the params
		{
			m_bCurrentlyDay= !m_bCurrentlyDay;
			if (m_bCurrentlyDay)
				UpdateSunrisePortion(GetYear(),GetMonth(),GetDay());
			else 
				UpdateSunsetPortion(GetYear(),GetMonth(),GetDay());
		}
		
		m_fACE_CurrentOutdoorTemperature = CalculateOutdoorTemperature(GetTimeOfTheDay());
		float timeStamp = GetTimeOfTheDay()+24*GetDay()-24;
		Print(timeStamp);
		Print(m_fACE_CurrentOutdoorTemperature);
	}
	
	//------------------------------------------------------------------------------------------------
	float CalculateOutdoorTemperature(float currentTime)
	{
		if (currentTime<m_fSunriseHour)//Post midnight, pre sunrise
		{
			return m_fTau + (m_fDailySunsetTemperature-m_fTau)*ACE_Math.Exp(-m_fExpDecay*(currentTime+24-m_fSunsetHour)/(24-m_fDayLength));
		}
		else if (currentTime<m_fSunsetHour)//sun is out, post sunrise pre sunset
		{
			float m_fInputBuffer = Math.PI*(currentTime-m_fSunriseHour)/(m_fDayLength+2*m_fAlpha);
			return m_fDailyTemperatureMinimum + (m_fDailyTemperatureMaximum - m_fDailyTemperatureMinimum) * Math.Sin(m_fInputBuffer);
		} 
		else //sun is set, pre midnight
		{
			return m_fTau + (m_fDailySunsetTemperature-m_fTau)*ACE_Math.Exp(-m_fExpDecay*(currentTime-m_fSunsetHour)/(24-m_fDayLength));
		} 
	}
	
	//------------------------------------------------------------------------------------------------
	float ACE_GetCurrentOutdoorTemperature()
	{
		return m_fACE_CurrentOutdoorTemperature;
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateSunrisePortion(int year, int month, int day)
	{
		m_fPeakTemperatureHour = Math.Lerp(m_fMonthlyPeakTemperatureHour[month-1],m_fMonthlyPeakTemperatureHour[month%12],(day-0.999999)/31);
		m_fDailyTemperatureMaximum = Math.Lerp(m_fMonthlyDailyHighTemperature[month-1],m_fMonthlyDailyHighTemperature[month%12],(day-0.999999)/31);
		GetSunriseHour(m_fSunriseHour); GetSunsetHour(m_fSunsetHour);
		m_fDayLength = m_fSunsetHour-m_fSunriseHour;
		m_fAlpha = m_fPeakTemperatureHour - (m_fSunriseHour+m_fSunsetHour)/2;
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateSunsetPortion(int year, int month, int day)
	{
		m_fDailySunsetTemperature = m_fACE_CurrentOutdoorTemperature;
		
		ACE_AddDaysToDate(year, month, day, 1); //Get tommorow's date
		GetSunriseHourForDate(year, month, day, GetCurrentLatitude(), GetCurrentLongitude(), GetTimeZoneOffset(), GetDSTOffset(),  m_fSunriseHourPrime);
		m_fDailyTemperatureMinimum = Math.Lerp(m_fMonthlyDailyLowTemperature[month-1],m_fMonthlyDailyLowTemperature[(month)%12],(day-0.999999)/31);
		m_fExpResultPrime = ACE_Math.Exp(-m_fExpDecay*(m_fSunriseHourPrime-m_fSunsetHour)/(24-m_fDayLength));
		m_fTau = (m_fDailyTemperatureMinimum - m_fDailySunsetTemperature*m_fExpResultPrime)/(1-m_fExpResultPrime);
	}
}
