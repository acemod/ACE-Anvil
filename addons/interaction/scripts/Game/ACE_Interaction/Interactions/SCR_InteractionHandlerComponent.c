//------------------------------------------------------------------------------------------------
//! This component allows the player to interact with their environment.
modded class SCR_InteractionHandlerComponent : InteractionHandlerComponent
{
	protected ChimeraCharacter m_pACE_Character;
	protected bool m_bACE_IsManualNearbyAvailableContextOverridden = false;
	
	//------------------------------------------------------------------------------------------------
	//! Show nearby interactions when a gadget is equipped
	override protected bool ShouldBeEnabled(SCR_NearbyContextDisplayMode displayMode, ChimeraCharacter character, bool playerCameraOnly = true)
	{
		if (super.ShouldBeEnabled(displayMode, character, playerCameraOnly))
		{
			m_bACE_IsManualNearbyAvailableContextOverridden = false;
			return true;
		}
		
		// Disallow out of player camera when true
		if (playerCameraOnly)
		{
			CameraManager cameraManager = GetGame().GetCameraManager();
			if (cameraManager && !PlayerCamera.Cast(cameraManager.CurrentCamera()))
				return false;
		}
		
		m_bACE_IsManualNearbyAvailableContextOverridden = ACE_HasGadgetInHand();
		return m_bACE_IsManualNearbyAvailableContextOverridden;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnControlledEntityChanged(IEntity from, IEntity to)
	{
		super.OnControlledEntityChanged(from, to);
		m_pACE_Character = ChimeraCharacter.Cast(to);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ACE_HasGadgetInHand()
	{
		CharacterControllerComponent charController = m_pACE_Character.GetCharacterController();
		if (!charController)
			return false;
		
		return charController.IsGadgetInHands();
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_GetManualNearbyAvailableContextOverride()
	{
		return m_bACE_IsManualNearbyAvailableContextOverridden;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Only show contexts that have gadget-specific actions
	void ACE_GetNearbyAvailableContextOverrideList(out notnull array<UserActionContext> contexts)
	{
		array<UserActionContext> unfilteredContexts = {};
		GetNearbyAvailableContextList(unfilteredContexts);
		contexts.Reserve(unfilteredContexts.Count());
		
		foreach (UserActionContext context :  unfilteredContexts)
		{
			array<BaseUserAction> actions = {};
			context.GetActionsList(actions);
			
			foreach (BaseUserAction action : actions)
			{
				if ((SCR_HealingUserAction.Cast(action)
					|| SCR_HealSupportStationAction.Cast(action)
					|| SCR_RepairAtSupportStationAction.Cast(action)
					|| SCR_RefuelAtSupportStationAction.Cast(action)
					|| ACE_BaseGadgetUserAction.Cast(action)
					) && action.CanBeShown(m_pACE_Character))
				{
					contexts.Insert(context);
					break;
				}
			}
		}
	}
}
