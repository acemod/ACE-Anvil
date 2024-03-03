//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode : BaseGameMode
{
	//------------------------------------------------------------------------------------------------
	//! Release carried player if they are killed or the carrier is killed
	protected override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		ACE_Carrying_Helper.ReleaseFromCarrier(playerEntity);
		ACE_Carrying_Helper.ReleaseCarried(playerEntity);
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
	};
	
	//------------------------------------------------------------------------------------------------
	//! Release carried player if they disconnected or the carrier disconnected
	protected override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		ACE_Carrying_Helper.ReleaseFromCarrier(player);
		ACE_Carrying_Helper.ReleaseCarried(player);
		super.OnPlayerDisconnected(playerId, cause, timeout);
	};
};
