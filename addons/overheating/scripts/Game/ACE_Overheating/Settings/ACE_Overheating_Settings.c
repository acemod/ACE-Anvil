//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Overheating_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "1.0", desc: "Scales the heat transferred to the barrel by a shot. The larger, the faster the temperature of the barrel increases.", params: "0 inf")]
	float m_fHeatingScale;
	
	[Attribute(defvalue: "1.0", desc: "Scale for how quickly a weapon cools down. Cooling rate is multiplied by this scale.", params: "0 inf")]
	float m_fDefaultCoolingScale;
	
	[Attribute(defvalue: "1.3", desc: "Scale for how quickly a weapon cools down for open bolt weapons.", params: "0 inf")]
	float m_fOpenBoltCoolingScale;
	
	[Attribute(defvalue: "20", desc: "Scale for how quickly a weapon cools down in water.", params: "0 inf")]
	float m_fWaterCoolingScale;
	
	[Attribute(defvalue: "1.0", desc: "Scale for how likely a weapon jams.", params: "0 inf")]
	float m_fJamChanceScale;
	
	[Attribute(defvalue: "723.15", desc: "Temperature at which ammo cooks off [K].", params: "0 inf")]
	float m_fGunpowderAutoignitionTemperature;
	
	// MassScaledTemperatureRateConstant [g/s] = HeatTransferCoefficient [J/(s*m^2*K)] * SurfaceArea [m^2] / SpecificHeat [J/(g*K)]
	[Attribute(defvalue: "0.52863", desc: "Mass-scaled temperature rate constant for heat exchange between barrel and bullet [g/s]. The larger, the faster a chambered bullet will heat up to barrel temperature.", params: "0 inf")]
	float m_fBarrel2BulletMassScaledTemperatureRateConstant;
	
	/*****
	[Attribute(defvalue: "0.1", desc: "Probability for clearing jam to fail.", params: "0 1")]
	float m_fClearJamFailureChance;
	*****/
}
