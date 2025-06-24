//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	
	//------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState);
		
		// OnLifeStateChanged sometimes gets triggered without a change in state
		if (!Replication.IsServer() || previousLifeState == newLifeState)
			return;
		
		switch (newLifeState)
		{
			case ECharacterLifeState.INCAPACITATED:
			{
				m_pACE_Medical_DamageManager.ACE_Medical_ScheduleSecondChanceDeactivation();
				break;
			}
			
			case ECharacterLifeState.ALIVE:
			{
				m_pACE_Medical_DamageManager.ACE_Medical_ClearSecondChanceHistory();
				break;
			}
		}
	}
}
