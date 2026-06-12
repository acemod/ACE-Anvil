//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_EarthMagneticFieldConfig
{
	// Data obtained from https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfgrid?lat1=-90&lat2=90&lon1=-180&lon2=179&latStepSize=2.5&lonStepSize=2.5&magneticComponent=d&key=REDACTED&resultFormat=csv
	[Attribute("Declination values in degrees, ordered latitude-major from latitude 90 to -90 and longitude -180 to <180.")]
	protected ref array<float> m_aDeclinations;
	
	[Attribute(defvalue: "2.5", desc: "Grid step size in degrees for both latitude and longitude.")]
	protected float m_fStepSize;
	
	//------------------------------------------------------------------------------------------------
	float ComputeDeclination(float latitude, float longitude)
	{
		int xCount = 360 / m_fStepSize;
		int yCount = 180 / m_fStepSize + 1;
		
		if (m_aDeclinations.Count() != xCount * yCount)
		{
			Debug.Error(string.Format("Bad length of m_aDeclinations. %1 instead of %2.", m_aDeclinations.Count(), xCount * yCount));
			return 0;
		}
		
		float x = (180 + longitude) / m_fStepSize;
		int x0 = (int)Math.Floor(x) % xCount;
		int x1 = (x0 + 1) % xCount;
		float tx = Math.Mod(x, 1);
		
		float y = (90 - latitude) / m_fStepSize;
		int y0 = Math.Floor(y);
		int y1 = Math.Min(y0 + 1, yCount - 1);
		float ty = Math.Mod(y, 1);
		
		int i00 = GetGridIndex(x0, y0);
		int i10 = GetGridIndex(x1, y0);
		int i01 = GetGridIndex(x0, y1);
		int i11 = GetGridIndex(x1, y1);
		
		return ACE_Math.Bilerp(m_aDeclinations[i00], m_aDeclinations[i10], m_aDeclinations[i01], m_aDeclinations[i11], tx, ty);
	}
	
	//------------------------------------------------------------------------------------------------
	protected int GetGridIndex(int x, int y)
	{
		return y * (360 / m_fStepSize) + x;
	}
}

