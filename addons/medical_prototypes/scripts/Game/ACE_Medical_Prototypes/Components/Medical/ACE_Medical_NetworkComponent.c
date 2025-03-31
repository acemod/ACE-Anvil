//------------------------------------------------------------------------------------------------
class ACE_Medical_NetworkComponentClass: ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Class attached to player controller 
class ACE_Medical_NetworkComponent : ScriptComponent
{
	protected SCR_PlayerController m_pPlayerController;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_pPlayerController = SCR_PlayerController.Cast(GetOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	void RequestVitalsNotification(ENotification type, SCR_ChimeraCharacter patient)
	{
		Rpc(RpcAsk_GetVitalsNotification, type, Replication.FindId(patient));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_GetVitalsNotification(ENotification type, RplId patientId)
	{
		SCR_ChimeraCharacter patient = SCR_ChimeraCharacter.Cast(Replication.FindItem(patientId));
		if (!patient)
			return;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			return;
		
		switch (type)
		{
			case ENotification.ACE_MEDICAL_PULSE_RESULT:
			{
				SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type, cardiovascularComponent.GetHeartRate());
				break;
			}
			
			case ENotification.ACE_MEDICAL_BLOOD_PRESSURE_RESULT:
			{
				Tuple2<float, float> pressures = cardiovascularComponent.GetBloodPressures();
				SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type, pressures.param2 * ACE_Medical_CardiovascularComponent.KPA2MMHG, pressures.param1 * ACE_Medical_CardiovascularComponent.KPA2MMHG);
				break;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Allows clients to request an update of the vitals data
	void RequestUpdateVitalsData(SCR_ChimeraCharacter patient)
	{
		Rpc(RpcAsk_UpdateVitalsData, Replication.FindId(patient));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_UpdateVitalsData(RplId patientId)
	{
		SCR_ChimeraCharacter patient = SCR_ChimeraCharacter.Cast(Replication.FindItem(patientId));
		if (!patient)
			return;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			return;
		
		Rpc(RpcDo_UpdateVitalsDataOwner, patientId, cardiovascularComponent.GetHeartRate(), cardiovascularComponent.GetMeanArterialPressure(), cardiovascularComponent.GetPulsePressure());
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_UpdateVitalsDataOwner(RplId patientId, float heartRate, float meanArterialPressure, float pulsePressure)
	{
		SCR_ChimeraCharacter patient = SCR_ChimeraCharacter.Cast(Replication.FindItem(patientId));
		if (!patient)
			return;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			return;
		
		cardiovascularComponent.SetHeartRate(heartRate);
		cardiovascularComponent.SetMeanArterialPressure(meanArterialPressure);
		cardiovascularComponent.SetPulsePressure(pulsePressure);
	}
}
