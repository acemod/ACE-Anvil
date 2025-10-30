//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected ACE_Medical_BrainHitZone m_pACE_Medical_BrainHitZone;
	protected float m_fACE_Medical_BloodFlowScale = 1;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Transition to cardiac arrest
	override void ACE_Medical_OnSecondChanceGranted()
	{
		// Only handle first second chance
		if (!m_bACE_Medical_WasSecondChanceGranted)
		{
			ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(GetOwner().FindComponent(ACE_Medical_VitalsComponent));
			if (vitalsComponent)
				vitalsComponent.SetVitalStateID(ACE_Medical_EVitalStateID.CARDIAC_ARREST);
		}
		
		super.ACE_Medical_OnSecondChanceGranted();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Reset vitals when fully healed (e.g. by GM)
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		super.FullHeal(ignoreHealingDOT);
		
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(GetOwner().FindComponent(ACE_Medical_VitalsComponent));
		if (vitalsComponent)
			vitalsComponent.Reset();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Unregister from medical systems when killed
	override void ACE_Medical_OnKilled()
	{
		super.ACE_Medical_OnKilled();
		
		ACE_Medical_VitalStatesSystem system = ACE_Medical_VitalStatesSystem.GetInstance(GetOwner().GetWorld());
		if (system)
			system.Unregister(SCR_ChimeraCharacter.Cast(GetOwner()));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Only allow resilience recovery in stable state
	override void ACE_Medical_UpdateResilienceRegenScale()
	{
		ACE_Medical_Circulation_Settings circulationSettings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
		if (!circulationSettings)
			return;
		
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(GetOwner().FindComponent(ACE_Medical_VitalsComponent));
		if (!vitalsComponent)
			return;
		
		// No recovery when vitals are not stable
		if (vitalsComponent.GetVitalStateID() != ACE_Medical_EVitalStateID.STABLE)
			m_fACE_Medical_ResilienceRegenScale = 0;
		else if (vitalsComponent.WasRevived())
			m_fACE_Medical_ResilienceRegenScale = circulationSettings.m_fMaxRevivalResilienceRecoveryScale;
		else
			m_fACE_Medical_ResilienceRegenScale = s_ACE_Medical_Core_Settings.m_fDefaultResilienceRegenScale;
		
		// Scale recovery scale by health of the brain hit zone
		m_fACE_Medical_ResilienceRegenScale *= m_pACE_Medical_BrainHitZone.GetHealthScaled();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Called alpha factor in KAM
	void ACE_Medical_SetBloodFlowScale(float scale)
	{
		m_fACE_Medical_BloodFlowScale = scale;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	float ACE_Medical_GetBloodFlowScale()
	{
		return m_fACE_Medical_BloodFlowScale;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Called by ACE_Medical_BrainHitZone.OnInit to initialize the hit zone
	void ACE_Medical_SetBrainHitZone(HitZone hitzone)
	{
		m_pACE_Medical_BrainHitZone = ACE_Medical_BrainHitZone.Cast(hitzone);
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Return the pain hit zone
	ACE_Medical_BrainHitZone ACE_Medical_GetBrainHitZone()
	{
		return m_pACE_Medical_BrainHitZone;
	}
}
