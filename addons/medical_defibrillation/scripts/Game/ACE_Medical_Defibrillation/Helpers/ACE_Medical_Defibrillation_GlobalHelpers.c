class ACE_Medical_Defibrillation_GlobalHelpers
{
	static ACE_Medical_NetworkComponent GetMedicalNetworkComponent(notnull SCR_ChimeraCharacter char)
	{
		PlayerManager playerMgr = GetGame().GetPlayerManager();
		if (!playerMgr)
			return null;
		
		int charId = playerMgr.GetPlayerIdFromControlledEntity(char);
		if (charId == 0)
			return null;
		
		SCR_PlayerController controller = SCR_PlayerController.Cast(playerMgr.GetPlayerController(charId));
		if (!controller)
			return null;
		
		ACE_Medical_NetworkComponent networkComponent = ACE_Medical_NetworkComponent.Cast(controller.FindComponent(ACE_Medical_NetworkComponent));
		if (!networkComponent)
			return null;
		
		return networkComponent;
	}
	
	//------------------------------------------------------------------------------------------------
	static float BpmToMs(float bpm)
	{
		if (bpm <= 0)
			return 0.0;
		
		return (60.0 / bpm) * 1000.0;
	}
	
	//------------------------------------------------------------------------------------------------
	static float MsToBpm(float ms)
	{
		if (ms <= 0)
			return 0.0;
		
		return 60.0 / (ms / 1000.0);
	}
}