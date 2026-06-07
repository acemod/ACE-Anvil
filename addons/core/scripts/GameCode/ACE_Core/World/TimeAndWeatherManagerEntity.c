//------------------------------------------------------------------------------------------------
modded class TimeAndWeatherManagerEntity : BaseTimeAndWeatherManagerEntity
{
	protected float m_fACE_UpdateFrequency; // One update per x seconds
	protected float m_fACE_UpdateTimer; // [s]
	
	//------------------------------------------------------------------------------------------------
	protected void TimeAndWeatherManagerEntity(IEntitySource src, IEntity parent)
	{
		src.Get("Update Frequency", m_fACE_UpdateFrequency);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);

		m_fACE_UpdateTimer -= timeSlice;
		if (m_fACE_UpdateTimer > 0)
			return;

		ACE_UpdateWeather(m_fACE_UpdateFrequency - m_fACE_UpdateTimer);
		m_fACE_UpdateTimer = m_fACE_UpdateFrequency;
	}
	
	//------------------------------------------------------------------------------------------------
	//! TODO: Replaced with BaseWeatherManagerEntity::UpdateWeather once it gets overridable
	void ACE_UpdateWeather(float timeSlice);
	
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
