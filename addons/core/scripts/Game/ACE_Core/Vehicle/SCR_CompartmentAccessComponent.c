
//------------------------------------------------------------------------------------------------
modded class SCR_CompartmentAccessComponent : CompartmentAccessComponent
{
	//------------------------------------------------------------------------------------------------
	void ACE_MoveOutVehicle(vector target_transform[4], bool sendIntoRagdoll = false, bool performWhenPaused = false)
	{
		Rpc(RpcDo_ACE_MoveOutVehicle_Owner, target_transform[0], target_transform[1], target_transform[2], target_transform[3], sendIntoRagdoll, performWhenPaused);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Transform is passed in parts, since fixed arrays are not supported by ScriptCallQueue::CallLater
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_MoveOutVehicle_Owner(vector v0, vector v1, vector v2, vector v3, bool sendIntoRagdoll, bool performWhenPaused)
	{
		vector target_transform[4] = {v0, v1, v2, v3};
		
		// Reschedule moving out if it failed, for instance, when called while moving in
		if (!GetOutVehicle_NoDoor(target_transform, sendIntoRagdoll, performWhenPaused) && GetVehicleIn(GetOwner()))
			GetGame().GetCallqueue().CallLater(RpcDo_ACE_MoveOutVehicle_Owner, 100, false, v0, v1, v2, v3, sendIntoRagdoll, performWhenPaused);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_GetOutVehicle(EGetOutType type, int doorInfoIndex, ECloseDoorAfterActions closeDoor, bool performWhenPaused)
	{
		Rpc(RpcDo_ACE_GetOutVehicle_Owner, type, doorInfoIndex, closeDoor, performWhenPaused);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_GetOutVehicle_Owner(EGetOutType type, int doorInfoIndex, ECloseDoorAfterActions closeDoor, bool performWhenPaused)
	{
		// Reschedule getting out if it failed, for instance, when called while getting in
		if (!GetOutVehicle(type, doorInfoIndex, closeDoor, performWhenPaused) && GetVehicleIn(GetOwner()))
			GetGame().GetCallqueue().CallLater(RpcDo_ACE_GetOutVehicle_Owner, 100, false, type, doorInfoIndex, closeDoor, performWhenPaused);
	}
}
