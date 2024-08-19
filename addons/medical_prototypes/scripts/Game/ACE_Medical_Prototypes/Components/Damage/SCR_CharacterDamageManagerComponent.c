//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	// ACE3 has this set to 50, but we lowered so minimum bleedout in cardiac arrest is about 30 instead of 12 seconds
	[Attribute(defvalue: "20", desc: "Maximum total bleeding rate at cardiac arrest due to gravity  [ml/s]", category: "ACE Medical")]
	protected float m_fACE_Medical_CardiacArrestMaxTotalBleedingRate;
	
	protected ACE_Medical_CardiovascularComponent m_pACE_Medical_CardiovascularComponent;
	protected ACE_Medical_MedicationManagerComponent m_pACE_Medical_MedicationManager;
	protected ACE_Medical_BrainHitZone m_pACE_Medical_BrainHitZone;
	
	protected int m_iACE_Medical_LastSecondChanceTickCount;
	protected float m_fACE_Medical_BloodFlowScale = 1;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		m_pACE_Medical_CardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(owner.FindComponent(ACE_Medical_CardiovascularComponent));
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		super.FullHeal(ignoreHealingDOT);
		
		//-----------------------------------------------------------------------------------------------------------
		//! TO DO: Replace solution once systems support inheritance
		/*
		IEntity owner = GetOwner();
		array<Managed> components = {};
		owner.FindComponents(ACE_Medical_BaseComponent, components);
		
		foreach (Managed component : components)
		{
			ACE_Medical_BaseComponent medicalComponent = ACE_Medical_BaseComponent.Cast(component);
			
			ACE_Medical_BaseSystem system = ACE_Medical_BaseSystem.GetInstance(medicalComponent.GetAssociatedSystemType());
			if (system)
				system.OnFullHeal(owner);
		}
		*/
		
		IEntity owner = GetOwner();
		
		ACE_Medical_BaseSystem system = ACE_Medical_BaseSystem.GetInstance(ACE_Medical_CardiovascularSystem);
		if (system)
			system.OnFullHeal(owner);
		
		ACE_Medical_BaseSystem2 system2 = ACE_Medical_BaseSystem2.GetInstance(ACE_Medical_MedicationComponent);
		if (system2)
			system2.OnFullHeal(owner);
		//-----------------------------------------------------------------------------------------------------------
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Add vitals as additional condition for unconsciousness
	override protected bool ShouldBeUnconscious()
	{
		if (super.ShouldBeUnconscious())
			return true;
		
		if (!m_pACE_Medical_CardiovascularComponent)
			return false;
		
		return (m_pACE_Medical_CardiovascularComponent.GetVitalState() >= ACE_Medical_EVitalState.CRITICAL);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Maximum total bleeding rate in ml/s
	float ACE_Medical_GetMaxTotalBleedingRate()
	{
		return GetBleedingScale() * m_fACE_Medical_BloodFlowScale * Math.Max(m_fACE_Medical_CardiacArrestMaxTotalBleedingRate, m_pACE_Medical_CardiovascularComponent.GetCardiacOutput() / 60);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Called alpha factor
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
