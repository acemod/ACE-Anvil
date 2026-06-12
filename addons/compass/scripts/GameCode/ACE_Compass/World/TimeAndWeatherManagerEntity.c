//------------------------------------------------------------------------------------------------
modded class TimeAndWeatherManagerEntity : BaseTimeAndWeatherManagerEntity
{
	[Attribute(defvalue: "{46B2C74B04BA64CE}Configs/ACE/EarthMagneticField.conf", desc: "Whether the magnetic declination should be overriden by the value specified in this manager.")]
	protected ResourceName m_sACE_EarthMagneticFieldConfigName;
	protected float m_fACE_MagneticDeclination = 0;
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		if (!GetGame().InPlayMode())
			return;
		
		ACE_EarthMagneticFieldConfig earthMagneticFieldConfig = SCR_ConfigHelperT<ACE_EarthMagneticFieldConfig>.GetConfigObject(m_sACE_EarthMagneticFieldConfigName);
		if (earthMagneticFieldConfig)
		{
			m_fACE_MagneticDeclination = earthMagneticFieldConfig.ComputeDeclination(GetCurrentLatitude(), GetCurrentLongitude());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns magnetic declination in degrees
	float ACE_GetMagneticDeclination()
	{
		return m_fACE_MagneticDeclination;
	}
}
