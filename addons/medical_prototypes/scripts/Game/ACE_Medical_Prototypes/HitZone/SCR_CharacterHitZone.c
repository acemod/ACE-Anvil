//------------------------------------------------------------------------------------------------
modded class SCR_CharacterHitZone : SCR_RegeneratingHitZone
{
	[Attribute(defvalue: "false", desc: "Whether the hit zone is vital and will kill the character when destroyed")]
	protected bool m_bACE_Medical_IsVital;
	
	[Attribute(defvalue: "false", desc: "Whether second chance is enabled for this hit zone (only relveant when hit zone is vital)")]
	protected bool m_bACE_Medical_SecondChanceEnabled;
	
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	protected ACE_Medical_CardiovascularComponent m_pACE_Medical_CardiovascularComponent;
	
	// Deactivation time is shared across all hit zones. This ensures that other vital hit zones that get destroyed within this time are also protected
	protected static const float ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS = 1000;
	protected static const float ACE_MEDICAL_SECOND_CHANCE_RESCALED_HEALTH = 0.1;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		m_pACE_Medical_CardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_CardiovascularComponent));
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Add handling for vital hit zones
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer())
			return;
		
		if (newState != EDamageState.DESTROYED)
			return;
		
		//! Add bleeding to physical hit zones that get destroyed
		m_pACE_Medical_DamageManager.AddBleedingEffectOnHitZone(this);
		
		if (!m_bACE_Medical_IsVital)
			return;
		
		bool hasLastSecondChanceExpired = System.GetTickCount(m_pACE_Medical_DamageManager.ACE_Medical_GetLastSecondChanceTickCount()) >= ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS;
		if (!m_bACE_Medical_SecondChanceEnabled || (m_pACE_Medical_CardiovascularComponent.IsInCardiacArrest() && hasLastSecondChanceExpired))
		{
			m_pACE_Medical_DamageManager.Kill(m_pACE_Medical_DamageManager.GetInstigator());
		}
		// Handle second chance
		else
		{
			SetHealthScaled(ACE_MEDICAL_SECOND_CHANCE_RESCALED_HEALTH);
			m_pACE_Medical_CardiovascularComponent.SetVitalState(ACE_Medical_EVitalState.CARDIAC_ARREST);
			
			if (hasLastSecondChanceExpired)
				m_pACE_Medical_DamageManager.ACE_Medical_OnSecondChanceTriggered(this);
		}
	}
}
