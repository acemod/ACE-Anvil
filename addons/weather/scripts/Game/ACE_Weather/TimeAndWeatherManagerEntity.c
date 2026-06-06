modded class TimeAndWeatherManagerEntity : BaseTimeAndWeatherManagerEntity 
{
	//Model using https://discord.com/channels/976165959041679380/1509719021908398121/1512238279070716037
	float m_fCurrentOutdoorTemperature=293;
	float m_fUpdateInterval = 30; // One update per x seconds
	float m_fNextUpdate = 3;
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
	
	void Init(){
		m_bCurrentlyDay = m_fSunriseHour<GetTimeOfTheDay()<m_fSunsetHour;
		if (m_bCurrentlyDay)//Init during day
		{
			m_fDailyTemperatureMinimum = Math.Lerp(m_fMonthlyDailyLowTemperature[GetMonth()-1],m_fMonthlyDailyLowTemperature[GetMonth()%12],(GetDay()-1)/30);
 			UpdateSunrisePortion(GetYear(),GetMonth(),GetDay());
		} 
		else 
		{
			GetSunriseHour(m_fSunriseHour); GetSunsetHour(m_fSunsetHour);
			int year = GetYear();int month = GetMonth(); int day = GetDay();
			GetDayXFromDate(year,month,day,1); //Get tmr's date 
			GetSunsetHourForDate(year,month,day,GetCurrentLatitude(), GetCurrentLongitude(), GetTimeZoneOffset(), GetDSTOffset(),  m_fSunsetHour);//Workaround for pre-midnight night inits
			m_fDayLength = m_fSunsetHour-m_fSunriseHour;
			m_fCurrentOutdoorTemperature = 283;//Gets loaded into sunset temp
			
			UpdateSunsetPortion(GetYear(),GetMonth(),GetDay());
		}
		m_bInitialized=true;
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		GetCurrentOutdoorTemperature();
	}
	
	void CalculateOutdoorTemperature(float currentTime)
	{
		if (m_fSunriseHour<currentTime<m_fSunsetHour != m_bCurrentlyDay)//Update the params
		{
			m_bCurrentlyDay = m_fSunriseHour<currentTime<m_fSunsetHour;
			if (m_bCurrentlyDay)
				UpdateSunrisePortion(GetYear(),GetMonth(),GetDay());
			else 
				UpdateSunsetPortion(GetYear(),GetMonth(),GetDay());
		}
		if (currentTime<m_fSunriseHour)//Post midnight, pre sunrise
		{
			m_fCurrentOutdoorTemperature = m_fTau + (m_fDailySunsetTemperature-m_fTau)*ACE_Math.Exp(-m_fExpDecay*(currentTime+24-m_fSunsetHour)/(24-m_fDayLength));
		}
		else if (currentTime<m_fSunsetHour)//sun is out, post sunrise pre sunset
		{
			float m_fInputBuffer = Math.PI*(currentTime-m_fSunriseHour)/(m_fDayLength+2*m_fAlpha);
			m_fCurrentOutdoorTemperature = m_fDailyTemperatureMinimum + (m_fDailyTemperatureMaximum - m_fDailyTemperatureMinimum) * Math.Sin(m_fInputBuffer);
		} 
		else //sun is set, pre midnight
		{
			m_fCurrentOutdoorTemperature = m_fTau + (m_fDailySunsetTemperature-m_fTau)*ACE_Math.Exp(-m_fExpDecay*(currentTime-m_fSunsetHour)/(24-m_fDayLength));
		} 
	}
	
	float GetCurrentOutdoorTemperature()
	{
		if (GetEngineTime()<m_fNextUpdate)
			return m_fCurrentOutdoorTemperature;
		if (!m_bInitialized)
			Init();
		CalculateOutdoorTemperature(GetTimeOfTheDay());
		float currentTime = GetTimeOfTheDay()+24*GetDay()-24;
		Print(currentTime);
		Print(m_fCurrentOutdoorTemperature);
		
		
		m_fNextUpdate = GetEngineTime()+m_fUpdateInterval;
		return m_fCurrentOutdoorTemperature;
	}
	
	void UpdateSunrisePortion(int year, int month, int day)
	{
		m_fPeakTemperatureHour = Math.Lerp(m_fMonthlyPeakTemperatureHour[month-1],m_fMonthlyPeakTemperatureHour[month%12],(day-1)/30);
		m_fDailyTemperatureMaximum = Math.Lerp(m_fMonthlyDailyHighTemperature[month-1],m_fMonthlyDailyHighTemperature[month%12],(day-1)/30);
		m_fAlpha = m_fPeakTemperatureHour - (m_fSunriseHour+m_fSunsetHour)/2;
		GetSunriseHour(m_fSunriseHour); GetSunsetHour(m_fSunsetHour);
		m_fDayLength = m_fSunsetHour-m_fSunriseHour;
	}
	
	void UpdateSunsetPortion(int year, int month, int day)
	{
		m_fDailySunsetTemperature = m_fCurrentOutdoorTemperature;
		
		GetDayXFromDate(year,month,day,1);//Get tommorow's date
		GetSunriseHourForDate(year, month, day, GetCurrentLatitude(), GetCurrentLongitude(), GetTimeZoneOffset(), GetDSTOffset(),  m_fSunriseHourPrime);
		m_fDailyTemperatureMinimum = Math.Lerp(m_fMonthlyDailyLowTemperature[month-1],m_fMonthlyDailyLowTemperature[(month)%12],(day-1)/30);
		m_fExpResultPrime = ACE_Math.Exp(-m_fExpDecay*(m_fSunriseHourPrime-m_fSunsetHour)/(24-m_fDayLength));
		m_fTau = (m_fDailyTemperatureMinimum - m_fDailySunsetTemperature*m_fExpResultPrime)/(1-m_fExpResultPrime);
	}

	void GetDayXFromDate(inout int year, inout int month, inout int day, int offset)
	{
	    int sign = offset.Sign();
	
	    while (offset != 0)
	    {
	        day += sign;
	        offset -= sign;
	
	        if (!CheckValidDate(year, month, day))
	        {
	            if (sign > 0)
	            {
	                if (month != 12)
	                {
	                    month += 1;
	                }
	                else
	                {
	                    month = 1;
	                    year += 1;
	                }
	
	                day = 1;
	            }
	            else
	            {
	                day = 31;
	
	                if (month == 1)
	                {
	                    month = 12;
	                    year -= 1;
	                }
	                else
	                {
	                    month -= 1;
	                }
	
	                while (!CheckValidDate(year, month, day))
	                {
	                    day -= 1;
	                }
	            }
	        }
	    }
	}
}