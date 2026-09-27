class ACE_Medical_Defibrillation_ConversionHelper
{
	//------------------------------------------------------------------------------------------------
	static float BpmToMs(float bpm)
	{
		if (bpm <= 0)
			return 0.0;
		
		return (60.0 / bpm) * 1000.0;
	}
	
	//------------------------------------------------------------------------------------------------
	static float MsToBpm(float ms)
	{
		if (ms <= 0)
			return 0.0;
		
		return 60.0 / (ms / 1000.0);
	}
}