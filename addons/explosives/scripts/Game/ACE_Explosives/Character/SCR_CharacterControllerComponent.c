//------------------------------------------------------------------------------------------------
//! Add registration for dead man's switches to be called when character gets incapacitated or killed
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	protected ref array<ACE_Explosives_DeadManSwitchGadgetComponent> m_aACE_Explosives_RegisteredDeadManSwitches = {};
	
	//------------------------------------------------------------------------------------------------
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState, bool isJIP)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState, isJIP);
		
		if (!Replication.IsServer() || newLifeState == ECharacterLifeState.ALIVE)
			return;
		
		foreach (ACE_Explosives_DeadManSwitchGadgetComponent trigger : m_aACE_Explosives_RegisteredDeadManSwitches)
		{
			trigger.OnOwnerIncapacitatedServer();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Explosives_RegisterDeadManSwitch(ACE_Explosives_DeadManSwitchGadgetComponent trigger)
	{
		m_aACE_Explosives_RegisteredDeadManSwitches.Insert(trigger);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Explosives_UnregisterDeadManSwitch(ACE_Explosives_DeadManSwitchGadgetComponent trigger)
	{
		m_aACE_Explosives_RegisteredDeadManSwitches.RemoveItem(trigger);
	}
}
