//------------------------------------------------------------------------------------------------
//! Class attached to player controller
modded class ACE_Medical_NetworkComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	override protected bool GetPatientStateNotificationData(ACE_Medical_CharacterContext patientContext, ENotification type, out int param1, out int param2)
	{
		if (super.GetPatientStateNotificationData(patientContext, type, param1, param2))
			return true;
		
		switch (type)
		{
			case ENotification.ACE_MEDICAL_TEMPERATURE_RESULT:
			{
				//TODO: use the actual constant later :p
				param1 = (patientContext.m_pVitals.GetTemperature()-273)*100;//*100 is Neccessary to get two decimal points of precision
				return true;
			}
			
		}
		
		return false;
	}
}
