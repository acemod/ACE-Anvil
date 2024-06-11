modded class SCR_PlayerController
{
	//------------------------------------------------------------------------------------------------
	//! Initialize ACE Medical on player controlled characters
	override void OnControlledEntityChanged(IEntity from, IEntity to)
	{
		super.OnControlledEntityChanged(from, to);
		
		// OnControlledEntityChanged runs on owner too, but we only want to call the init on the server
		if (Replication.IsRunning() && !Replication.IsServer())
			return;
		
		//! Do not initialize for GM controlled AI
		if (!to || IsPossessing())
			return;
		
		const SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(to.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager)
			damageManager.ACE_Medical_Initialize();
	}
}