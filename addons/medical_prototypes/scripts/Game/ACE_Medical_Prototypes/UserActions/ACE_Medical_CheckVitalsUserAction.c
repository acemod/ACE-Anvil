//------------------------------------------------------------------------------------------------
class ACE_Medical_CheckVitalsUserAction : ScriptedUserAction
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of the notification", enums: ParamEnumArray.FromEnum(ENotification))]
	protected ENotification m_eNotificationType;
	
	protected SCR_ChimeraCharacter m_pOwner;
		
	//------------------------------------------------------------------------------------------------
	//! Called when object is initialized and registered to actions manager
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_pOwner = SCR_ChimeraCharacter.Cast(pOwnerEntity);
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

		CharacterControllerComponent userController = userChar.GetCharacterController();
		if (!userController)
			return false;
		
		if (userController.IsSwimming() || userController.IsFalling() || userController.IsGadgetInHands())
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
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Called when someone tries to perform the continuous action, user entity is typically character
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;
		
		ACE_Medical_NetworkComponent networkComponent = ACE_Medical_NetworkComponent.Cast(playerController.FindComponent(ACE_Medical_NetworkComponent));
		if (!networkComponent)
			return;
		
		networkComponent.RequestVitalsNotification(m_eNotificationType, SCR_ChimeraCharacter.Cast(pOwnerEntity));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Should only be executed on the local user
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
}
