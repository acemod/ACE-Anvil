//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	[RplProp()]
	protected bool m_bACE_Captive_HasSurrendered = false;
	[RplProp()]
	protected bool m_bACE_Captive_IsCaptive = false;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Captives_SetHasSurrendered(bool hasSurrendered)
	{
		m_bACE_Captive_HasSurrendered = hasSurrendered;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Captives_HasSurrendered()
	{
		return m_bACE_Captive_HasSurrendered;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Captives_SetIsCaptive(bool isCaptive)
	{
		m_bACE_Captive_IsCaptive = isCaptive;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Captives_IsCaptive()
	{
		return m_bACE_Captive_IsCaptive;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Captives_EscortCaptive(IEntity escort)
	{
		if (!m_bACE_Captive_IsCaptive)
			return;
		
		ACE_Captives_TiedHelperCompartment helper = ACE_Captives_TiedHelperCompartment.Cast(GetOwner().GetParent());
		if (!helper)
			return;
		
		ACE_CarriableEntityComponent carriable = ACE_CarriableEntityComponent.Cast(helper.FindComponent(ACE_CarriableEntityComponent));
		if (!carriable)
			return;
		
		carriable.Carry(escort);
	}
}
