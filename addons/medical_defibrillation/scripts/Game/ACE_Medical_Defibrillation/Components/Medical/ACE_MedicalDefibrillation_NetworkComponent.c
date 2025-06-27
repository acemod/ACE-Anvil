//------------------------------------------------------------------------------------------------
class ACE_MedicalDefibrillation_NetworkComponentClass: ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Class attached to player controller 
class ACE_MedicalDefibrillation_NetworkComponent : ScriptComponent
{
	protected SCR_PlayerController m_PlayerController;
	
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
	override protected void OnPostInit(IEntity owner)
	{
	    super.OnPostInit(owner);
	    m_PlayerController = SCR_PlayerController.Cast(GetOwner());
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
	    PrintFormat("ACE_MedicalDefibrillation_NetworkComponent::RpcAsk_GetDefibrillatorNotification | Server Execution: %1", Replication.IsServer());
	    
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
	            SCR_NotificationsComponent.SendToPlayer(m_PlayerController.GetPlayerId(), type, patientID);
	            break;
	        }
	        case ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTCONNECTED:
	        {
	            int playerID = GetGame().GetPlayerManager().GetPlayerIdFromEntityRplId(patientID);
	            SCR_NotificationsComponent.SendToPlayer(m_PlayerController.GetPlayerId(), type, playerID);
	            break;
	        }
	        case ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTDISCONNECTED:
	        {
	            SCR_NotificationsComponent.SendToPlayer(m_PlayerController.GetPlayerId(), type);
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
	            SCR_NotificationsComponent.SendToPlayer(m_PlayerController.GetPlayerId(), type, cardiovascularComponent.GetShocksDelivered());
	            break;
	        }
	        case ENotification.ACE_MEDICALDEFIBRILLATION_ANALYSED:
	        {
	            SCR_NotificationsComponent.SendToPlayer(m_PlayerController.GetPlayerId(), type);
	            break;
	        }
	        case ENotification.ACE_MEDICALDEFIBRILLATION_CHARGED:
	        {
	            SCR_NotificationsComponent.SendToPlayer(m_PlayerController.GetPlayerId(), type);
	            break;
	        }
	    }
	}
}
