
//------------------------------------------------------------------------------------------------
modded class SCR_CompartmentAccessComponent : CompartmentAccessComponent
{
	//------------------------------------------------------------------------------------------------
	void ACE_MoveOutVehicle(vector target_transform[4], bool sendIntoRagdoll = false, bool performWhenPaused = false)
	{
		Rpc(ACE_MoveOutVehicle_Owner, target_transform, sendIntoRagdoll, performWhenPaused);
	}
	
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void ACE_MoveOutVehicle_Owner(vector target_transform[4], bool sendIntoRagdoll, bool performWhenPaused)
	{
		this.GetOutVehicle_NoDoor(target_transform, false, false);
	}
}
