
//------------------------------------------------------------------------------------------------
modded class SCR_CompartmentAccessComponent : CompartmentAccessComponent
{
	// Parameters for rescheduling failed get in/out
	protected static const int ATTEMPT_TIMEOUT = 500;
	protected static const int MAX_ATTEMPTS = 20;
	
	//------------------------------------------------------------------------------------------------
	void ACE_MoveOutVehicle(vector target_transform[4], bool sendIntoRagdoll = false, bool performWhenPaused = false)
	{
		Rpc(RpcDo_ACE_MoveOutVehicle_Owner, target_transform[0], target_transform[1], target_transform[2], target_transform[3], sendIntoRagdoll, performWhenPaused, 0);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Transform is passed in parts, since fixed arrays are not supported by ScriptCallQueue::CallLater
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_MoveOutVehicle_Owner(vector v0, vector v1, vector v2, vector v3, bool sendIntoRagdoll, bool performWhenPaused, int iAttempt)
	{
		vector target_transform[4] = {v0, v1, v2, v3};
		
		if (GetOutVehicle_NoDoor(target_transform, sendIntoRagdoll, performWhenPaused))
			return;
		
		if (!GetVehicleIn(GetOwner()))
			return;
		
		if (++iAttempt >= MAX_ATTEMPTS)
		{
			Debug.Error("Maximum number of attempts exceeded!");
			return;
		}
		
		// Reschedule moving out if it failed, for instance, when called while moving in
		GetGame().GetCallqueue().CallLater(RpcDo_ACE_MoveOutVehicle_Owner, ATTEMPT_TIMEOUT, false, v0, v1, v2, v3, sendIntoRagdoll, performWhenPaused, iAttempt);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_GetOutVehicle(EGetOutType type, int doorInfoIndex, ECloseDoorAfterActions closeDoor, bool performWhenPaused)
	{
		Rpc(RpcDo_ACE_GetOutVehicle_Owner, type, doorInfoIndex, closeDoor, performWhenPaused, 0);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_GetOutVehicle_Owner(EGetOutType type, int doorInfoIndex, ECloseDoorAfterActions closeDoor, bool performWhenPaused, int iAttempt)
	{
		if (GetOutVehicle(type, doorInfoIndex, closeDoor, performWhenPaused))
			return;
		
		if (!GetVehicleIn(GetOwner()))
			return;
		
		if (++iAttempt >= MAX_ATTEMPTS)
		{
			Debug.Error("Maximum number of attempts exceeded!");
			return;
		}
		
		// Reschedule getting out if it failed, for instance, when called while getting in
		GetGame().GetCallqueue().CallLater(RpcDo_ACE_GetOutVehicle_Owner, ATTEMPT_TIMEOUT, false, type, doorInfoIndex, closeDoor, performWhenPaused, iAttempt);
	}
}
