//------------------------------------------------------------------------------------------------
class ACE_Medical_DefibrillatorNetworkComponentClass: ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Class attached to player controller 
class ACE_Medical_DefibrillatorNetworkComponent : ScriptComponent
{
	protected SCR_PlayerController m_pPlayerController;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_pPlayerController = SCR_PlayerController.Cast(GetOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	void RequestDefibrillatorNotification(ENotification type, IEntity defibrillator, SCR_ChimeraCharacter patient)
	{
		Rpc(RpcAsk_GetDefibrillatorNotification,
			type,
			ACE_Medical_DefibrillationReplicationHelper.GetRplIdByEntity(defibrillator),
			ACE_Medical_DefibrillationReplicationHelper.GetRplIdByEntity(patient));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_GetDefibrillatorNotification(ENotification type, RplId defibrillatorID, RplId patientID)
	{
		PrintFormat("ACE_Medical_DefibrillatorNetworkComponent::RpcAsk_GetDefibrillatorNotification | Server Execution: %1", Replication.IsServer());
		
		SCR_ChimeraCharacter patient = SCR_ChimeraCharacter.Cast(ACE_Medical_DefibrillationReplicationHelper.GetEntityByRplId(patientID));
		if (!patient)
			return;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			return;
		
		IEntity defibrillator = IEntity.Cast(ACE_Medical_DefibrillationReplicationHelper.GetEntityByRplId(defibrillatorID));
		if (!defibrillator)
			return;
		
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(defibrillator.FindComponent(ACE_Medical_DefibrillatorComponent));
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
			case ENotification.ACE_MEDICALDEFIBRILLATION_SHOCKDELIVERED:
			{
				SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type, cardiovascularComponent.GetShocksDelivered());
				break;
			}
		}
	}
	
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
}
