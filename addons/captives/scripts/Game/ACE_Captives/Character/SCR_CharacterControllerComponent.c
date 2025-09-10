//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	[RplProp()]
	protected bool m_bACE_Captives_HasSurrendered = false;
	[RplProp()]
	protected bool m_bACE_Captives_IsCaptive = false;
	
	[RplProp()]
	protected ACE_Captives_ETakeCaptiveRequirement m_eACE_Captives_TakePlayerCaptiveRequirements;
	[RplProp()]
	protected ACE_Captives_ETakeCaptiveRequirement m_eACE_Captives_TakeAICaptiveRequirements;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		if (!Replication.IsServer())
			return;
		
		ACE_Captives_Settings settings = ACE_SettingsHelperT<ACE_Captives_Settings>.GetModSettings();
		if (!settings)
			return;
		
		m_eACE_Captives_TakePlayerCaptiveRequirements = settings.m_eTakePlayerCaptiveRequirements;
		m_eACE_Captives_TakeAICaptiveRequirements = settings.m_eTakeAICaptiveRequirements;
		Replication.BumpMe();
	}
	
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
		ACE_Captives_CaptiveSystem system = ACE_Captives_CaptiveSystem.GetInstance();
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
	//! Friend setter of ACE_Captives_SurrenderHelperCompartment
	void ACE_Captives_SetHasSurrendered(bool hasSurrendered)
	{
		m_bACE_Captives_HasSurrendered = hasSurrendered;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Friend setter of ACE_Captives_CaptiveSystem
	void ACE_Captives_SetIsCaptive(bool isCaptive)
	{
		m_bACE_Captives_IsCaptive = isCaptive;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Captives_CanBeTakenCaptive(IEntity captor)
	{
		if (m_bACE_Captives_IsCaptive || m_bACE_IsCarried)
			return false;
		
		// Incapacitated and surrendered characters can always be tied
		if (GetLifeState() == ECharacterLifeState.INCAPACITATED || m_bACE_Captives_HasSurrendered)
			return true;
		
		ACE_Captives_ETakeCaptiveRequirement requirements = m_eACE_Captives_TakeAICaptiveRequirements;
		
		if (EntityUtils.IsPlayer(GetOwner()))
			requirements = m_eACE_Captives_TakePlayerCaptiveRequirements;
		
		if (requirements == ACE_Captives_ETakeCaptiveRequirement.SURRENDER)
			return false;
		
		if (!GetWeaponManagerComponent().GetCurrentWeapon())
			return true;
		
		if (requirements == ACE_Captives_ETakeCaptiveRequirement.UNARMED)
			return false;
		
		// For FROM_BEHIND_FOR_ARMED, armed units can be captured from behind.
		Animation ownerAnim = GetOwner().GetAnimation();
		vector rightHandTransform[4], leftHandTransform[4];
		ownerAnim.GetBoneMatrix(ownerAnim.GetBoneIndex("RightHand"), rightHandTransform);
		ownerAnim.GetBoneMatrix(ownerAnim.GetBoneIndex("LeftHand"), leftHandTransform);
		vector ownerDir = GetOwner().GetWorldTransformAxis(2);
		ownerDir[1] = 0;
		// Take the mean of hand positions as owner origin
		vector relCaptorPos = captor.GetOrigin() - GetOwner().CoordToParent(0.5 * (rightHandTransform[3] + leftHandTransform[3]));
		relCaptorPos[1] = 0;
		float dot = vector.DotXZ(ownerDir, relCaptorPos);
		
		// Dot product has to be negative to be behind owner
		if (dot >= 0)
			return false;
		
		// 0.4 corresponds to a sector angle of 101.54°
		return dot * dot / ownerDir.LengthSq() / relCaptorPos.LengthSq() >= 0.4;
	}
}
