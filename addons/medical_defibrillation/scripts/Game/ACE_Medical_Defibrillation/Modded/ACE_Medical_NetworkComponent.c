modded class ACE_Medical_NetworkComponent
{
	//------------------------------------------------------------------------------------------------
	void RequestDefibNotification(ENotification type, SCR_ChimeraCharacter patient = null)
	{
		Rpc(RpcAsk_GetDefibNotification,
			type,
			ACE_Medical_Defibrillation_ReplicationHelper.GetRplIdByEntity(patient)
		);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_GetDefibNotification(ENotification type, RplId patientId)
	{
		SCR_PlayerController ownerPlayerController = SCR_PlayerController.Cast(GetOwner());
		
		// TODO: In future, add patient data to message
		switch (type)
		{	
			case ENotification.ACE_MEDICAL_DEFIBRILLATION_UNABLETOCONNECT:
			{
				SCR_NotificationsComponent.SendToPlayer(ownerPlayerController.GetPlayerId(), type);
				break;
			}
					
			case ENotification.ACE_MEDICAL_DEFIBRILLATION_CONNECTED:
			{
				SCR_NotificationsComponent.SendToPlayer(ownerPlayerController.GetPlayerId(), type);
				break;
			}
			
			case ENotification.ACE_MEDICAL_DEFIBRILLATION_DISCONNECTED:
			{
				SCR_NotificationsComponent.SendToPlayer(ownerPlayerController.GetPlayerId(), type);
				break;
			}
			
			case ENotification.ACE_MEDICAL_DEFIBRILLATION_SHOCKDELIVERED:
			{
				SCR_NotificationsComponent.SendToPlayer(ownerPlayerController.GetPlayerId(), type);
				break;
			}
		}
	}
}