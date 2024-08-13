//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	// ACE3 has this set to 50, but we lowered so minimum bleedout in cardiac arrest is about 30 instead of 12 seconds
	[Attribute(defvalue: "20", desc: "Maximum total bleeding rate at cardiac arrest due to gravity  [ml/s]", category: "ACE Medical")]
	protected float m_fACE_Medical_CardiacArrestMaxTotalBleedingRate;
	
	protected ACE_Medical_CardiovascularSystemComponent m_pACE_Medical_CardiovascularSystem;
	protected ACE_Medical_MedicationManagerComponent m_pACE_Medical_MedicationManager;
	protected ACE_Medical_BrainHitZone m_pACE_Medical_BrainHitZone;
	
	protected int m_iACE_Medical_LastSecondChanceTickCount;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		m_pACE_Medical_CardiovascularSystem = ACE_Medical_CardiovascularSystemComponent.Cast(owner.FindComponent(ACE_Medical_CardiovascularSystemComponent));
		m_pACE_Medical_MedicationManager = ACE_Medical_MedicationManagerComponent.Cast(owner.FindComponent(ACE_Medical_MedicationManagerComponent));
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		super.FullHeal(ignoreHealingDOT);
		m_pACE_Medical_CardiovascularSystem.ResetVitalsToDefault();
		m_pACE_Medical_MedicationManager.Clear();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Add vitals as additional condition for unconsciousness
	override protected bool ShouldBeUnconscious()
	{
		if (super.ShouldBeUnconscious())
			return true;
		
		if (!m_pACE_Medical_CardiovascularSystem)
			return false;
		
		return (m_pACE_Medical_CardiovascularSystem.GetVitalState() >= ACE_Medical_EVitalState.CRITICAL);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Maximum total bleeding rate in ml/s
	float ACE_Medical_GetMaxTotalBleedingRate()
	{
		return GetBleedingScale() * Math.Max(m_fACE_Medical_CardiacArrestMaxTotalBleedingRate, m_pACE_Medical_CardiovascularSystem.GetCardiacOutput() / 60);
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
	
	//-----------------------------------------------------------------------------------------------------------
	void ACE_Medical_OnSecondChanceTriggered(SCR_HitZone hitZone)
	{
		m_iACE_Medical_LastSecondChanceTickCount = System.GetTickCount();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	int ACE_Medical_GetLastSecondChanceTickCount()
	{
		return m_iACE_Medical_LastSecondChanceTickCount;
	}
}
