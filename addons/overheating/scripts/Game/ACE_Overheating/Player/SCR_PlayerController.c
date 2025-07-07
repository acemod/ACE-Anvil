//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController : PlayerController
{
	//------------------------------------------------------------------------------------------------
	override protected void OnControlledEntityChanged(IEntity from, IEntity to)
	{
		super.OnControlledEntityChanged(from, to);
		
		ACE_Overheating_JammingSystem system = ACE_Overheating_JammingSystem.GetInstance();
		if (!system)
			return;
		
		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (!rpl)
			return;
		
		if (from)
			system.UnregisterCharacter(ChimeraCharacter.Cast(from), rpl.IsOwner());
		
		if (to)
			system.RegisterCharacter(ChimeraCharacter.Cast(to), rpl.IsOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_RequestClearJam(ACE_Overheating_MuzzleJamComponent jamComponent)
	{
		Rpc(RplAsk_ACE_Overheating_ClearJamServer, Replication.FindItemId(jamComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RplAsk_ACE_Overheating_ClearJamServer(RplId jamComponentID)
	{
		ACE_Overheating_MuzzleJamComponent jamComponent = ACE_Overheating_MuzzleJamComponent.Cast(Replication.FindItem(jamComponentID));
		if (jamComponent)
			jamComponent.SetState(false);
	}
}
