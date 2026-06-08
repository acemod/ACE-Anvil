//------------------------------------------------------------------------------------------------
class ACE_WeatherHelper
{
	//------------------------------------------------------------------------------------------------
	static float GetAirTemperatureForEntity(IEntity entity)
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE;
		
		TimeAndWeatherManagerEntity weatherManager = world.GetTimeAndWeatherManager();
		if (!weatherManager)
			return ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE;
		
		float altitude = entity.GetOrigin()[1];
		return weatherManager.ACE_GetAirTemperature(altitude);
	}
}
