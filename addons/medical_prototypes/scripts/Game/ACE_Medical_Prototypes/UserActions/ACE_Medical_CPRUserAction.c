//------------------------------------------------------------------------------------------------
class ACE_Medical_CPRUserAction : SCR_ScriptedUserAction
{
	protected SCR_ChimeraCharacter m_pOwner;
	protected IEntity m_pCurrentUser;
	
	protected ACE_Medical_CardiovascularComponent m_pCardiovascularComponent;
	
	//------------------------------------------------------------------------------------------------
	//! Called when object is initialized and registered to actions manager
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		GetGame().GetCallqueue().Call(DelayedInit, pOwnerEntity, pManagerComponent);
	}
	
	protected void DelayedInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_pOwner = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		m_pCardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_CardiovascularComponent));
	}

	//------------------------------------------------------------------------------------------------
	//! Can this action be shown in the UI to the provided user entity?
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;

		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;

		if (userChar.IsInVehicle())
			return false;

		CharacterControllerComponent userController = userChar.GetCharacterController();
		if (!userController)
			return false;

		if (userController.IsSwimming() || userController.IsFalling())
			return false;
		
		CharacterControllerComponent ownerController = m_pOwner.GetCharacterController();
		if (!ownerController)
			return false;
		
		if (!ownerController.IsUnconscious())
			return false;
		
		CharacterAnimationComponent ownerAnimation = m_pOwner.GetAnimationComponent();
		if (!ownerAnimation)
			return false;
		
		if (ownerAnimation.IsRagdollActive())
			return false;
		
		// Lock this action: We can only see it when no one else is performing it or when we are already performing it
		if (m_pCardiovascularComponent.IsCPRPerformed() && (user != m_pCurrentUser))
			return false;
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Method called from scripted interaction handler when an action is started (progress bar appeared)
	//! \param pUserEntity The entity that started performing this action
	override void OnActionStart(IEntity pUserEntity)
	{
		super.OnActionStart(pUserEntity);
		m_pCurrentUser = pUserEntity;
		
		if (Replication.IsServer())
			m_pCardiovascularComponent.SetIsCPRPerformed(true);
	}

	//------------------------------------------------------------------------------------------------
	//! Method called when the action is interrupted/canceled.
	//! \param pUserEntity The entity that was performing this action prior to interruption
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.OnActionCanceled(pOwnerEntity, pUserEntity);
		m_pCurrentUser = null;
		
		if (Replication.IsServer())
			m_pCardiovascularComponent.SetIsCPRPerformed(false);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the progress of this action in seconds.
	override float GetActionProgressScript(float fProgress, float timeSlice)
	{
		return fProgress + timeSlice;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clients should not run the action
	override bool CanBroadcastScript()
	{
		return false;
	}
}
