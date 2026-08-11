//------------------------------------------------------------------------------------------------
modded class SCR_PoisonDamageEffect : SCR_DotDamageEffect
{
	protected float m_ACE_CBRN_InitialDPS; // Initial DPS to be reapplied when switching from decay to build up
	protected SCR_CompartmentAccessComponent m_ACE_CBRN_CompartmentAccessComponent;
	protected bool m_bACE_CBRN_PenetrateWatertightCompartments = false;
	protected ACE_CBRN_CharacterProtectionComponent m_ACE_CBRN_ProtectionComponent;
	protected ACE_CBRN_ECharacterProtectedArea m_eACE_CBRN_RequiredProtection = ACE_CBRN_ECharacterProtectedArea.AIRWAYS;
	
	//------------------------------------------------------------------------------------------------
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		IEntity entity = dmgManager.GetOwner();
		if (!entity)
			return true;
		
		if (m_ACE_CBRN_InitialDPS <= 0)
			m_ACE_CBRN_InitialDPS = GetDPS();
		
		m_ACE_CBRN_CompartmentAccessComponent = SCR_CompartmentAccessComponent.Cast(entity.FindComponent(SCR_CompartmentAccessComponent));
		m_ACE_CBRN_ProtectionComponent = ACE_CBRN_CharacterProtectionComponent.Cast(entity.FindComponent(ACE_CBRN_CharacterProtectionComponent));
		
		// Disable DPS when there is no build up
		if (ACE_CBRN_ShouldDecay(dmgManager))
		{
			m_fLocalDPSValue = 0;
			SetDPS(m_fLocalDPSValue);
			ApplyEffect(dmgManager);
		}
		
		return super.HijackDamageEffect(dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		ACE_CBRN_UpdateDecay(timeSlice, dmgManager);
		timeSlice = GetAccurateTimeSlice(timeSlice);
		DotDamageEffectTimerToken token = UpdateTimer(timeSlice, dmgManager);
		DealCustomDot(GetAffectedHitZone(), m_fLocalDPSValue * timeSlice, token, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update whether poison should build up or decay
	protected void ACE_CBRN_UpdateDecay(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		bool decaying = ACE_CBRN_ShouldDecay(dmgManager);
		if (decaying == m_bIsDecaying)
			return;
		
		m_bIsDecaying = decaying;
		
		// Reset DPS when switching from decay back to build up
		if (!m_bIsDecaying && GetDPS() < m_ACE_CBRN_InitialDPS)
		{
			m_fLocalDPSValue = m_ACE_CBRN_InitialDPS;
			SetDPS(m_fLocalDPSValue);
		}
		
		ApplyEffect(dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return whether poison should build up or decay
	protected bool ACE_CBRN_ShouldDecay(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		if (GetMaxDuration() > 0)
			return true;
		
		BaseCompartmentSlot slot = m_ACE_CBRN_CompartmentAccessComponent.GetCompartment();
		if (!m_bACE_CBRN_PenetrateWatertightCompartments && slot && slot.GetIsWaterTight())
			return true;

		if (m_ACE_CBRN_ProtectionComponent.HasRequiredProtection(m_eACE_CBRN_RequiredProtection))
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set required protection state to not get affected by the poison
	void ACE_CBRN_SetRequiredProtection(ACE_CBRN_ECharacterProtectedArea protectionState)
	{
		m_eACE_CBRN_RequiredProtection = protectionState;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set whether wateright vehicle compartments protect from posioning
	void ACE_CBRN_SetPenetrateWatertightCompartments(bool shouldPenetrate)
	{
		m_bACE_CBRN_PenetrateWatertightCompartments = shouldPenetrate;
	}
}
