//------------------------------------------------------------------------------------------------
//! Used for hit zones that should kill or trigger second chance when destroyed
//! Create an instance in SCR_HitZone.OnInit and store a ref
class ACE_Medical_VitalHitZoneHelper : Managed
{
	protected SCR_CharacterDamageManagerComponent m_pDamageManager;
	protected ACE_Medical_CardiovascularSystemComponent m_pCardiovascularSystem;
	protected bool m_bSecondChanceEnabled;
	protected float m_fSecondChanceRescaledHealth;
	// Deactivation time is shared across all hit zones. This ensures that other vital hit zones that get destroyed within this time are also protected
	protected const float SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS = 1000;
	// Reset to full health as players would otherwise die too fast from brain damage received from cardiac arrest
	protected const float SECOND_CHANCE_RESCALED_BRAIN_HEALTH = 1;
	protected const float SECOND_CHANCE_RESCALED_HEALTH = 0.1;
	
	//------------------------------------------------------------------------------------------------
	//! Initialize member variables
	void ACE_Medical_VitalHitZoneHelper(IEntity ownerEntity, GenericComponent managerComponent, SCR_HitZone hitZone)
	{
		m_pDamageManager = SCR_CharacterDamageManagerComponent.Cast(managerComponent);
		m_pCardiovascularSystem = ACE_Medical_CardiovascularSystemComponent.Cast(ownerEntity.FindComponent(ACE_Medical_CardiovascularSystemComponent));
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings)
		{
			// Settings for brain hit zone
			if (ACE_Medical_BrainHitZone.Cast(hitZone))
			{
				m_bSecondChanceEnabled = settings.m_bSecondChanceOnHeadEnabled;
				m_fSecondChanceRescaledHealth = SECOND_CHANCE_RESCALED_BRAIN_HEALTH;
			}
			// Settings for head hit zone
			else if (SCR_CharacterHeadHitZone.Cast(hitZone))
			{
				m_bSecondChanceEnabled = settings.m_bSecondChanceOnHeadEnabled;
				m_fSecondChanceRescaledHealth = SECOND_CHANCE_RESCALED_HEALTH;
			}
			// Settings for other physical hit zones
			else
			{
				m_bSecondChanceEnabled = settings.m_bSecondChanceEnabled;
				m_fSecondChanceRescaledHealth = SECOND_CHANCE_RESCALED_HEALTH;
			}
		}
		
		hitZone.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Handle second chance and death
	protected void OnDamageStateChanged(SCR_HitZone hitZone)
	{
		if (hitZone.GetDamageState() != ECharacterDamageState.DESTROYED)
			return;
		
		bool hasLastSecondChanceExpired = System.GetTickCount(m_pDamageManager.ACE_Medical_GetLastSecondChanceTickCount()) >= SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS;
		
		if (!m_bSecondChanceEnabled || (m_pCardiovascularSystem.IsInCardiacArrest() && hasLastSecondChanceExpired))
		{
			m_pDamageManager.Kill(m_pDamageManager.GetInstigator());
		}
		// Handle second chance
		else
		{
			hitZone.SetHealthScaled(m_fSecondChanceRescaledHealth);
			m_pCardiovascularSystem.SetVitalState(ACE_Medical_EVitalState.CARDIAC_ARREST);
			
			if (hasLastSecondChanceExpired)
				m_pDamageManager.ACE_Medical_OnSecondChanceTriggered(hitZone);
		}
	}
}
