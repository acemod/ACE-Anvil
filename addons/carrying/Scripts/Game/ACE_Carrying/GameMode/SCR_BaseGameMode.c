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
			if (ACE_CarryingTools.IsCarrier(char))
			{
				ACE_CarryingTools.ReleaseFromCarrier(char);
			}
			else if (ACE_CarryingTools.IsCarried(char))
			{
				ACE_CarryingTools.ReleaseCarried(char);
			}
		}
		
		super.OnPlayerDisconnected(playerId, cause, timeout);
	};
};
