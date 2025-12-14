//------------------------------------------------------------------------------------------------
//! Animation command to force a character to ragdoll
//! Use CharacterAnimationComponent::SetCurrentCommand to apply it
class ACE_CharacterCommandRagdoll : CharacterCommandScripted
{
	protected ChimeraCharacter m_pCharacter;
	protected CharacterControllerComponent m_pCharController;
	protected float m_fDuration;
	
	//------------------------------------------------------------------------------------------------
	void ACE_CharacterCommandRagdoll(BaseAnimPhysComponent pAnimPhysComponent, ChimeraCharacter pCharacter, float duration = -1)
	{
		m_pCharacter = pCharacter;
		m_pCharController = m_pCharacter.GetCharacterController();
		m_fDuration = duration;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActivate()
	{
		//m_pCharController.Ragdoll(true);
		
		if (m_fDuration >= 0)
			GetGame().GetCallqueue().CallLater(Terminate, m_fDuration * 1000);
	}

	//------------------------------------------------------------------------------------------------
	void Terminate()
	{
		SetFlagFinished(true);
	}
}
