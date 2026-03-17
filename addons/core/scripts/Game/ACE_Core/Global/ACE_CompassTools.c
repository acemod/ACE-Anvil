//------------------------------------------------------------------------------------------------
//! Tools for dealing with bearings and caridnals
class ACE_CompassTools
{
	protected static ref const array<string> s_aCardinals = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
	
	//------------------------------------------------------------------------------------------------
	//! Returns the bearing for a given world transform
	//! \param[in] transform
	//! \return bearing in degrees
	static float GetBearingFromTransform(vector transform[4])
	{
		return transform[2].ToYaw();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the closest cardinal from a given bearing
	//! \param[in] bearing in degrees
	//! \return cardinal
	static string GetCardinalFromBearing(float bearing)
	{
		int i = Math.Round(bearing / 22.5);
		if (i > 15)
			i = 0;
		
		return s_aCardinals[i];
	}
}
