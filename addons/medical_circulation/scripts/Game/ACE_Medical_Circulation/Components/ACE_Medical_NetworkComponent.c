//------------------------------------------------------------------------------------------------
class ACE_Medical_NetworkComponentClass: ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Class attached to player controller
class ACE_Medical_NetworkComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	void RequestPatientStateNotification(ENotification type, SCR_ChimeraCharacter patient)
	{
		Rpc(RpcAsk_SendPatientStateNotification, type, Replication.FindId(patient));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_SendPatientStateNotification(ENotification type, RplId patientId)
	{
		SCR_ChimeraCharacter patient = SCR_ChimeraCharacter.Cast(Replication.FindItem(patientId));
		if (!patient)
			return;
		
		ACE_Medical_CharacterContext patientContext = new ACE_Medical_CharacterContext(patient);
		
		int param1, param2;
		if (!GetPatientStateNotificationData(patientContext, type, param1, param2))
			return;
		
		SCR_PlayerController ownerPlayerController = SCR_PlayerController.Cast(GetOwner());
		SCR_NotificationsComponent.SendToPlayer(ownerPlayerController.GetPlayerId(), type, param1, param2);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool GetPatientStateNotificationData(ACE_Medical_CharacterContext patientContext, ENotification type, out int param1, out int param2)
	{
		switch (type)
		{
			case ENotification.ACE_MEDICAL_PULSE_RESULT:
			{
				param1 = patientContext.m_pVitals.GetHeartRate();
				return true;
			}
			
			case ENotification.ACE_MEDICAL_BLOOD_PRESSURE_RESULT:
			{
				Tuple2<float, float> pressures = patientContext.m_pVitals.GetBloodPressures();
				param1 = pressures.param2 * ACE_PhysicalConstants.KPA2MMHG;
				param2 = pressures.param1 * ACE_PhysicalConstants.KPA2MMHG;
				return true;
			}
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Allows clients to request an update of the data of a patient
	void RequestUpdatePatientData(SCR_ChimeraCharacter patient)
	{
		Rpc(RpcAsk_UpdatePatientData, Replication.FindId(patient));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_UpdatePatientData(RplId patientID)
	{
		SCR_ChimeraCharacter patient = SCR_ChimeraCharacter.Cast(Replication.FindItem(patientID));
		if (patient)
			Rpc(RpcDo_UpdatePatientDataOwner, patientID, ACE_Medical_PatientData.Create(patient));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_UpdatePatientDataOwner(RplId patientID, ACE_Medical_PatientData data)
	{
		SCR_ChimeraCharacter patient = SCR_ChimeraCharacter.Cast(Replication.FindItem(patientID));
		if (patient)
			data.Apply(patient);
	}
}
