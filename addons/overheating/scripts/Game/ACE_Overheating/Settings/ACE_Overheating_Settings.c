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
	
	[Attribute(defvalue: "200", desc: "Scale for how quickly a weapon cools down in water.", params: "0 inf")]
	float m_fWaterCoolingScale;
	
	[Attribute(defvalue: "1.0", desc: "Scale for how likely a weapon jams.", params: "0 inf")]
	float m_fJamChanceScale;
	
	[Attribute(defvalue: "0.1", desc: "Probability for clearing jam to fail.", params: "0 1")]
	float m_fClearJamFailureChance;
	
	// The reference data (A. Hameed et al., Defence Technology 2014, 10, 86–91) with which our model was fitted used Bullseye propellant (433.15 K), but for ACE, we take the upper limit of Ball propellant (473.15 K)
	[Attribute(defvalue: "473.15", desc: "Temperature at which ammo can cook off [K].", params: "0 1300")]
	float m_fGunpowderAutoignitionTemperature;
	
	// Fit to M60 model, such that cook-off progress will exactly reach 1.0 and then stop progressing for the next bullet and T_barrel_final=35°C when firing 100 RPM for 2 minutes and then cease fire for 15 minutes.
	[Attribute(defvalue: "125.63", desc: "Base heat transfer coefficient for cooling of barrel from other channels than radiation and convection from wind and rain.", params: "0 inf")]
	float m_fBaseHeatTransferCoefficient;
	
	// MassScaledTemperatureRateConstant [g/s] = HeatTransferCoefficient [J/(s*m^2*K)] * SurfaceArea [m^2] / SpecificHeat [J/(g*K)]
	// The fit of reference data (A. Hameed et al., Defence Technology 2014, 10, 86–91) would be 30.4587377 g/s, but we assume that heat transfer from barrel to bullet is far less efficient than in the experimental configuration of the paper
	// "*1/15" is closer to the tail at high T of ACE3's cook-off time vs barrel temperature
  	[Attribute(defvalue: "2.031", desc: "Mass-scaled temperature rate constant for heat exchange between barrel and bullet [g/s]. The larger, the faster a chambered bullet will heat up to barrel temperature.", params: "0 inf")]
	float m_fBarrel2BulletMassScaledTemperatureRateConstant;
	
	[Attribute(defvalue: "1.0", desc: "Scales the additional temperature-dependent dispersion of shots. Set to 0.0 to turn it off.", params: "0 inf")]
	float m_fMuzzleDispersionScale;
	
	[Attribute(defvalue: "500", desc: "Temperature above which barrel starts smoking [K].", params: "0 1300")]
	float m_fMinSmokeTemperature;
	
	[Attribute(defvalue: "800", desc: "Temperature above which barrel starts glowing [K].", params: "0 1300")]
	float m_fMinGlowTemperature;
}
