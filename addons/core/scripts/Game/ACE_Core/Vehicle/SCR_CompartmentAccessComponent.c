
//------------------------------------------------------------------------------------------------
modded class SCR_CompartmentAccessComponent : CompartmentAccessComponent
{
	protected bool m_bACE_IsRequestingGettingIn = false;
	protected bool m_bACE_IsRequestingGettingOut = false;
	
	//------------------------------------------------------------------------------------------------
	void ACE_GetInVehicle(notnull IEntity vehicle, BaseCompartmentSlot compartment = null, bool forceTeleport = true, int doorInfoIndex = -1, ECloseDoorAfterActions closeDoor = ECloseDoorAfterActions.INVALID, bool performWhenPaused = false)
	{
		if (!compartment)
			compartment = FindFreeAndAccessibleCompartment(vehicle, ECompartmentType.CARGO, compartment);
		
		RplComponent vehicleRpl = RplComponent.Cast(vehicle.FindComponent(RplComponent));
		if (!vehicleRpl)
			return;
		
		ACE_SetIsRequestingGettingIn(true);
		Rpc(RpcDo_ACE_GetInVehicle_Owner, vehicleRpl.Id(), compartment.GetCompartmentSlotID(), compartment.GetCompartmentMgrID(), forceTeleport, doorInfoIndex, closeDoor, performWhenPaused);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_GetInVehicle_Owner(RplId vehicleID, int slotID, int mgrID, bool forceTeleport, int doorInfoIndex, ECloseDoorAfterActions closeDoor, bool performWhenPaused)
	{

		RplComponent vehicleRpl = RplComponent.Cast(Replication.FindItem(vehicleID));
		if (!vehicleRpl)
		{
			ACE_SetIsRequestingGettingIn(false);
			return;
		}
		
		IEntity vehicle = vehicleRpl.GetEntity();
		if (!vehicle)
		{
			ACE_SetIsRequestingGettingIn(false);
			return;
		}
		
		BaseCompartmentManagerComponent compartmentManager = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
		if (!compartmentManager)
		{
			ACE_SetIsRequestingGettingIn(false);
			return;
		}
		
		BaseCompartmentSlot compartment = compartmentManager.FindCompartment(slotID, mgrID);
		if (!compartment)
		{
			ACE_SetIsRequestingGettingIn(false);
			return;
		}
		
		GetInVehicle(vehicle, compartment, forceTeleport, doorInfoIndex, closeDoor, performWhenPaused);
		ACE_SetIsRequestingGettingIn(false);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_SetIsRequestingGettingIn(bool isRequested)
	{
		m_bACE_IsRequestingGettingIn = isRequested;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return;
		
		RplComponent rpl = ownerChar.GetRplComponent();
		if (!rpl)
			return;
		
		if (rpl.IsOwner())
			Rpc(RpcAsk_ACE_SetIsRequestingGettingIn_Server, isRequested);
		else if (rpl.Role() == RplRole.Authority)
			Rpc(RpcDo_ACE_SetIsRequestingGettingIn_Owner, isRequested);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_SetIsRequestingGettingIn_Server(bool isRequested)
	{
		m_bACE_IsRequestingGettingIn = isRequested;
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_SetIsRequestingGettingIn_Owner(bool isRequested)
	{
		m_bACE_IsRequestingGettingIn = isRequested;
	}
	
	//------------------------------------------------------------------------------------------------
	//! More reliable alternative to IsGettingIn, as it is already true during the request
	bool ACE_IsGettingIn()
	{
		return m_bACE_IsRequestingGettingIn || IsGettingIn();
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_MoveOutVehicle(vector target_transform[4], bool sendIntoRagdoll = false, bool performWhenPaused = false)
	{
		ACE_SetIsRequestingGettingOut(true);
		Rpc(RpcDo_ACE_MoveOutVehicle_Owner, target_transform[0], target_transform[1], target_transform[2], target_transform[3], sendIntoRagdoll, performWhenPaused);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Transform is passed in parts, since fixed arrays are not supported by ScriptCallQueue::CallLater
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_MoveOutVehicle_Owner(vector v0, vector v1, vector v2, vector v3, bool sendIntoRagdoll, bool performWhenPaused)
	{
		vector target_transform[4] = {v0, v1, v2, v3};
		GetOutVehicle_NoDoor(target_transform, sendIntoRagdoll, performWhenPaused);
		ACE_SetIsRequestingGettingOut(false);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_GetOutVehicle(EGetOutType type, int doorInfoIndex, ECloseDoorAfterActions closeDoor, bool performWhenPaused)
	{
		ACE_SetIsRequestingGettingOut(true);
		Rpc(RpcDo_ACE_GetOutVehicle_Owner, type, doorInfoIndex, closeDoor, performWhenPaused);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_GetOutVehicle_Owner(EGetOutType type, int doorInfoIndex, ECloseDoorAfterActions closeDoor, bool performWhenPaused)
	{
		GetOutVehicle(type, doorInfoIndex, closeDoor, performWhenPaused);
		ACE_SetIsRequestingGettingOut(false);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_SetIsRequestingGettingOut(bool isRequested)
	{
		m_bACE_IsRequestingGettingOut = isRequested;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return;
		
		RplComponent rpl = ownerChar.GetRplComponent();
		if (!rpl)
			return;
		
		if (rpl.IsOwner())
			Rpc(RpcAsk_ACE_SetIsRequestingGettingOut_Server, isRequested);
		else if (rpl.Role() == RplRole.Authority)
			Rpc(RpcDo_ACE_SetIsRequestingGettingOut_Owner, isRequested);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_SetIsRequestingGettingOut_Server(bool isRequested)
	{
		m_bACE_IsRequestingGettingOut = isRequested;
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_SetIsRequestingGettingOut_Owner(bool isRequested)
	{
		m_bACE_IsRequestingGettingOut = isRequested;
	}
	
	//------------------------------------------------------------------------------------------------
	//! More reliable alternative to IsGettingOut, as it is already true during the request
	bool ACE_IsGettingOut()
	{
		return m_bACE_IsRequestingGettingOut || IsGettingOut();
	}
}
