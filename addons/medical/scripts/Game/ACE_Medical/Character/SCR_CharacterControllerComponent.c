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

		// Add null check to avoid potential null reference errors
		if (!m_pACE_Medical_DamageManager)
		{
			Print("ACE Medical Damage Manager not found!", LogLevel.ERROR);
			return;
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Add/remove second chance when life state changes
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState);

		// Check if ACE Medical has been initialized
		if (!m_pACE_Medical_DamageManager.ACE_Medical_IsInitialized())
			return;

		// OnLifeStateChanged sometimes gets triggered without a change in state
		if (previousLifeState == newLifeState)
			return;

		switch (newLifeState)
		{
			case ECharacterLifeState.ALIVE:
				HandleAliveState();
				break;

			case ECharacterLifeState.INCAPACITATED:
				HandleIncapacitatedState();
				break;

			case ECharacterLifeState.DEAD:
				HandleDeadState();
				break;
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Handle logic when character is revived (ALIVE state)
	void HandleAliveState()
	{
		auto callQueue = GetGame().GetCallqueue();
		callQueue.Remove(m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance);
		m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance(true);
		m_pACE_Medical_DamageManager.ACE_Medical_SetSecondChanceTriggered(false);
	}

	//------------------------------------------------------------------------------------------------
	//! Handle logic when character is incapacitated (INCAPACITATED state)
	void HandleIncapacitatedState()
	{
		auto callQueue = GetGame().GetCallqueue();
		callQueue.CallLater(m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance, ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS, false, false);
	}

	//------------------------------------------------------------------------------------------------
	//! Handle logic when character is dead (DEAD state)
	void HandleDeadState()
	{
		auto callQueue = GetGame().GetCallqueue();
		callQueue.Remove(m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance);
		m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance(false);
	}
}

