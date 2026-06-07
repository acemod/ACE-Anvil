//------------------------------------------------------------------------------------------------
modded class TimeAndWeatherManagerEntity : BaseTimeAndWeatherManagerEntity
{
	//------------------------------------------------------------------------------------------------
	//! Converts time of the day at 0° longitude (e.g. returned by GetTimeOfTheDay()) to local time of 
	//! the day for the current longitude
	//! \param[in] time Time of the day in hours
	//! \return local time of the day in hours
	float ACE_ToLocalTimeOfTheDay(float time)
	{
		return SCR_Math.fmod(time + GetCurrentLongitude() / 15, 24);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Adds offset in days to date
	//! \param[inout] year Year of the date to update
	//! \param[inout] month Month of the date to update
	//! \param[inout] day Day of the date to update
	//! \param[in] offset Offset in days (can be negative)
	void ACE_AddDaysToDate(inout int year, inout int month, inout int day, int offset)
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
