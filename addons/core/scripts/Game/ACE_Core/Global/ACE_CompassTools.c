//------------------------------------------------------------------------------------------------
//! Tools for dealing with bearings and caridnals
class ACE_CompassTools
{
	protected static ref const array<string> s_aCardinals = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
	
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
		int i = Math.Round(bearing / 45);
		if (i > 7)
			i = 0;
		
		return s_aCardinals[i];
	}
}
