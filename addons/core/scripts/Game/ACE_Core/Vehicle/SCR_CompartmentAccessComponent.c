
//------------------------------------------------------------------------------------------------
modded class SCR_CompartmentAccessComponent : CompartmentAccessComponent
{
	//------------------------------------------------------------------------------------------------
	void ACE_MoveOutVehicle(vector target_transform[4], bool sendIntoRagdoll = false, bool performWhenPaused = false)
	{
		Rpc(ACE_MoveOutVehicle_Owner, target_transform, sendIntoRagdoll, performWhenPaused);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void ACE_MoveOutVehicle_Owner(vector target_transform[4], bool sendIntoRagdoll, bool performWhenPaused)
	{
		GetOutVehicle_NoDoor(target_transform, sendIntoRagdoll, performWhenPaused);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_OpenDoor(Vehicle vehicle, ECharacterDoorAnimType doorAnimType, int doorIndex)
	{
		RplComponent vehicleRpl = vehicle.GetRplComponent();
		if (!vehicleRpl)
			return;
		
		Rpc(ACE_OpenDoor_Owner, vehicleRpl.Id(), doorAnimType, doorIndex);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void ACE_OpenDoor_Owner(RplId vehicleID, ECharacterDoorAnimType doorAnimType, int doorIndex)
	{
		RplComponent vehicleRpl = RplComponent.Cast(Replication.FindItem(vehicleID));
		if (!vehicleRpl)
			return;
		
		OpenDoor(vehicleRpl.GetEntity(), doorAnimType, doorIndex);
	}
}
