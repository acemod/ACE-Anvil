modded class ACE_Medical_VitalsComponent : ACE_BaseComponent{
	//Kelvin = 273 + degrees celsius
	protected float body_temperature=20;//Core temperature of patient
	protected float environmental_temperature;//Environmental temperature around the patient, kelvin
	//Returns the CORE temperature, in K
	float GetTemperature(){
		return body_temperature;
	}
	
	float UpdateTemperature(){
	}
}