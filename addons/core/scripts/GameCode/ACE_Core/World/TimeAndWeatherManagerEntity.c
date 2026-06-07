//------------------------------------------------------------------------------------------------
modded class TimeAndWeatherManagerEntity : BaseTimeAndWeatherManagerEntity
{
	//------------------------------------------------------------------------------------------------
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
