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
			case ENotification.ACE_MEDICAL_SHOCKDELIVERED:
			{
				SCR_NotificationsComponent.SendToPlayer(m_pPlayerController.GetPlayerId(), type, cardiovascularComponent.GetShocksDelivered());
				break;
			}
		}
	}
}
