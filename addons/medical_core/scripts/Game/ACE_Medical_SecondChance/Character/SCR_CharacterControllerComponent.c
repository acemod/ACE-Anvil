//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState, bool isJIP)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState, isJIP);
		
		// OnLifeStateChanged sometimes gets triggered without a change in state
		if (!Replication.IsServer() || previousLifeState == newLifeState)
			return;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(ownerChar.GetDamageManager());
		if (!damageManager)
			return;
		
		switch (newLifeState)
		{
			case ECharacterLifeState.INCAPACITATED:
			{
				damageManager.ACE_Medical_ScheduleSecondChanceDeactivation();
				break;
			}
			
			case ECharacterLifeState.ALIVE:
			{
				damageManager.ACE_Medical_ClearSecondChanceHistory();
				break;
			}
		}
	}
}
