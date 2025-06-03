//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	
	//------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Reset second chance when waking up
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState);
		
		if (!Replication.IsServer())
			return;
		
		if (previousLifeState != ECharacterLifeState.INCAPACITATED || newLifeState != ECharacterLifeState.ALIVE)
			return;
		
		m_pACE_Medical_DamageManager.ACE_Medical_ClearSecondChanceHistory();
	}
}
