//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode : BaseGameMode
{
	//------------------------------------------------------------------------------------------------
	//! Release carried player if they disconnected or the carrier disconnected
	protected override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity char = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (char)
		{
			if (ACE_Carrying_Tools.IsCarrier(char))
			{
				ACE_Carrying_Tools.ReleaseFromCarrier(char);
			}
			else if (ACE_Carrying_Tools.IsCarried(char))
			{
				ACE_Carrying_Tools.ReleaseCarried(char);
			}
		}
		
		super.OnPlayerDisconnected(playerId, cause, timeout);
	};
};
