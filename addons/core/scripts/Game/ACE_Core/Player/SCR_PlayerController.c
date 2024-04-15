//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController : PlayerController
{
	//------------------------------------------------------------------------------------------------
	//! Request deletion of unreplicated entity from all machines
	//! Called from local player
	void ACE_DeleteEntityAtPosition(vector pos)
	{
		Rpc(RpcAsk_ACE_DeleteEntityAtPosition, pos);
	}

	//------------------------------------------------------------------------------------------------
	//! Request deletion of unreplicated entity from all machines
	//! Called from server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_ACE_DeleteEntityAtPosition(vector pos)
	{
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return;

		gameMode.ACE_DeleteEntitiesAtPositionsGlobal( { pos });
	}
}
