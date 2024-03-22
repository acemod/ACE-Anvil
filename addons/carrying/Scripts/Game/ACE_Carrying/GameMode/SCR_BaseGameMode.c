//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode : BaseGameMode
{
	//------------------------------------------------------------------------------------------------
	//! Release carried player if they disconnected or the carrier disconnected
	protected override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity char = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (!char)
			return;
		
		if (ACE_Carrying_Helper.IsCarrier(char))
			ACE_Carrying_Helper.ReleaseFromCarrier(char);
		
		if (ACE_Carrying_Helper.IsCarried(char))
			ACE_Carrying_Helper.ReleaseCarried(char);
		
		super.OnPlayerDisconnected(playerId, cause, timeout);
	};
};
