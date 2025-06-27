//------------------------------------------------------------------------------------------------
class ACE_MedicalDefibrillation_NetworkComponentClass: ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Class attached to player controller 
modded class ACE_Medical_NetworkComponent
{
	//------------------------------------------------------------------------------------------------
	static string GetCharacterName(IEntity entity)
	{
	    PlayerManager playerManager = GetGame().GetPlayerManager();
	    if (!playerManager)
	        return "Unknown";
	    
	    string name;
	    int id = playerManager.GetPlayerIdFromControlledEntity(entity);
	    if (id != 0)
	    {
	        name = playerManager.GetPlayerName(id);
	    }
	    
	    return name;
	}
	
	//------------------------------------------------------------------------------------------------
	void RequestDefibrillatorConnectPatient(GameEntity defibrillator, SCR_ChimeraCharacter patient, SCR_ChimeraCharacter user)
	{
		RplId defibrillatorId = ACE_ReplicationHelper.GetRplIdByEntity(defibrillator); // TODO: Keeps returning -1
		RplId patientId = ACE_ReplicationHelper.GetRplIdByEntity(patient);
		RplId userId = ACE_ReplicationHelper.GetRplIdByEntity(user);
		
		if (!defibrillatorId.IsValid())
			return;
		if (!patientId.IsValid())
			return;
		if (!userId.IsValid())
			return;
		
		Rpc(Rpc_ConnectPatient, defibrillatorId, patientId, userId);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void Rpc_ConnectPatient(RplId defibrillatorId, RplId patientId, RplId userId)
	{
		IEntity defibrillator = IEntity.Cast(Replication.FindItem(defibrillatorId));
		if (!defibrillator)
			return;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(defibrillator.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibComponent)
			return;
		
		IEntity patient = IEntity.Cast(Replication.FindItem(patientId));
		if (!patient)
			return;
		
		IEntity user = IEntity.Cast(Replication.FindItem(userId));
		if (!user)
			return;
		
		if(defibComponent.ConnectPatient(patient))
		{
			ENotification notification;
			if (EntityUtils.IsPlayer(patient))
				notification = ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTCONNECTED;
			else
				notification = ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTCONNECTED_AI;
			
			RequestDefibrillatorNotification(
				notification,
				GetOwner(),
				SCR_ChimeraCharacter.Cast(patient)
			);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void RequestDefibrillatorNotification(ENotification type, IEntity defibrillator, SCR_ChimeraCharacter patient = null)
	{
	    Rpc(RpcAsk_GetDefibrillatorNotification,
	        type,
	        ACE_ReplicationHelper.GetRplIdByEntity(defibrillator),
	        ACE_ReplicationHelper.GetRplIdByEntity(patient));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_GetDefibrillatorNotification(ENotification type, RplId defibrillatorID, RplId patientID)
	{
	    // PrintFormat("ACE_MedicalDefibrillation_NetworkComponent::RpcAsk_GetDefibrillatorNotification | Server Execution: %1", Replication.IsServer());
	    
	    IEntity defibrillator = IEntity.Cast(ACE_ReplicationHelper.GetEntityByRplId(defibrillatorID));
	    if (!defibrillator)
	        return;
	    
	    ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(defibrillator.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
	    if (!defibrillatorComponent)
	        return;
	    
	    switch (type)
	    {
	        case ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTCONNECTED_AI:
	        {
	            SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type, patientID);
	            break;
	        }
	        case ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTCONNECTED:
	        {
	            int playerID = GetGame().GetPlayerManager().GetPlayerIdFromEntityRplId(patientID);
	            SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type, playerID);
	            break;
	        }
	        case ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTDISCONNECTED:
	        {
	            SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type);
	            break;
	        }
	        case ENotification.ACE_MEDICALDEFIBRILLATION_SHOCKDELIVERED:
	        {
	            IEntity patient;
	            ACE_Medical_CardiovascularComponent cardiovascularComponent;
	            if(!defibrillatorComponent.GetConnectedPatient(patient, cardiovascularComponent))
	                break;
	            if (!cardiovascularComponent)
	                break;
	            SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type, cardiovascularComponent.GetShocksDelivered());
	            break;
	        }
	        case ENotification.ACE_MEDICALDEFIBRILLATION_ANALYSED:
	        {
	            SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type);
	            break;
	        }
	        case ENotification.ACE_MEDICALDEFIBRILLATION_CHARGED:
	        {
	            SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type);
	            break;
	        }
	    }
	}
}
