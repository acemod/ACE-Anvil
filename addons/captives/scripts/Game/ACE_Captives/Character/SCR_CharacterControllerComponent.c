//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	[RplProp()]
	protected bool m_bACE_Captives_HasSurrendered = false;
	[RplProp()]
	protected bool m_bACE_Captives_IsCaptive = false;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Captives_SetSurrender(bool hasSurrendered)
	{
		if (hasSurrendered)
			ACE_AnimationTools.AnimateWithHelperCompartment(ACE_EAnimationHelperID.SURRENDER, SCR_ChimeraCharacter.Cast(GetOwner()));
		else
			ACE_AnimationTools.TerminateHelperCompartment(GetOwner());
		
		ACE_Captives_SetHasSurrendered(hasSurrendered);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Captives_SetCaptive(bool isCaptive)
	{
		ACE_Captives_CaptiveSystem system = ACE_Captives_CaptiveSystem.GetInstance(GetOwner().GetWorld());
		if (!system)
			return;
		
		if (isCaptive)
			system.Register(SCR_ChimeraCharacter.Cast(GetOwner()));
		else
			system.Unregister(SCR_ChimeraCharacter.Cast(GetOwner()));
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Captives_EscortCaptive(SCR_ChimeraCharacter escort)
	{
		if (!m_bACE_Captives_IsCaptive)
			return;
		
		SCR_CharacterControllerComponent escortCharController = SCR_CharacterControllerComponent.Cast(escort.GetCharacterController());
		if (!escortCharController)
			return;
		
		escortCharController.ACE_Carry(ACE_AnimationTools.GetHelperCompartment(GetOwner()));
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Captives_HasSurrendered()
	{
		return m_bACE_Captives_HasSurrendered;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Captives_IsCaptive()
	{
		return m_bACE_Captives_IsCaptive;
	}
	
	//------------------------------------------------------------------------------------------------
	[Friend(ACE_Captives_SurrenderHelperCompartment)]
	protected void ACE_Captives_SetHasSurrendered(bool hasSurrendered)
	{
		m_bACE_Captives_HasSurrendered = hasSurrendered;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	[Friend(ACE_Captives_CaptiveSystem)]
	protected void ACE_Captives_SetIsCaptive(bool isCaptive)
	{
		m_bACE_Captives_IsCaptive = isCaptive;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Prevent captives from equipping gadgets in vehicles
	override bool GetCanEquipGadget(IEntity gadget)
	{
		if (m_bACE_Captives_IsCaptive)
			return false;
		
		return super.GetCanEquipGadget(gadget);
	}
}
