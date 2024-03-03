//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode : BaseGameMode
{
	//------------------------------------------------------------------------------------------------
	//! Release carried player if they are killed or the carrier is killed
	protected override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		CEN_CarrierSystem_Helper.ReleaseFromCarrier(playerEntity);
		CEN_CarrierSystem_Helper.ReleaseCarried(playerEntity);
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
	};
	
	//------------------------------------------------------------------------------------------------
	//! Release carried player if they disconnected or the carrier disconnected
	protected override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		CEN_CarrierSystem_Helper.ReleaseFromCarrier(player);
		CEN_CarrierSystem_Helper.ReleaseCarried(player);
		super.OnPlayerDisconnected(playerId, cause, timeout);
	};
};
