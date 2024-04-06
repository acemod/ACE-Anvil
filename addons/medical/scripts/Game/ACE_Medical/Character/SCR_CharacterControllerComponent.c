//------------------------------------------------------------------------------------------------
//! Introduce a second chance, which gets triggered when the character would have died without
//! falling unconscious.
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	protected const float ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS = 1000;
	
	//------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Add/remove second chance when life state changes
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState);
		
		// Only run if ACE Medical has been initialized for this character
		if (!m_pACE_Medical_DamageManager.ACE_Medical_IsInitialized())
			return;
		
		// OnLifeStateChanged sometimes gets triggered without a change in state
		if (previousLifeState == newLifeState)
			return;
		
		switch (newLifeState)
		{
			// Add second chance when revived
			case ECharacterLifeState.ALIVE:
			{
				GetGame().GetCallqueue().Remove(m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance);
				m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance(true);
				m_pACE_Medical_DamageManager.ACE_Medical_SetSecondChanceTrigged(false);
				break;
			}
			
			// Schedule removal of second chance when falling unconscious
			case ECharacterLifeState.INCAPACITATED:
			{
				GetGame().GetCallqueue().CallLater(m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance, ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS, false, false);
				break;
			}
			
			// Remove second chance when dead
			case ECharacterLifeState.DEAD:
			{
				GetGame().GetCallqueue().Remove(m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance);
				m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance(false);
				break;
			}
		}
	}
}