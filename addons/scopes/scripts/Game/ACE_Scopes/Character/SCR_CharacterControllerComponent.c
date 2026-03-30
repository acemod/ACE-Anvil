//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//------------------------------------------------------------------------------------------------
	protected override void OnControlledByPlayer(IEntity owner, bool controlled)
	{
		super.OnControlledByPlayer(owner, controlled);
		
		if (controlled && owner == SCR_PlayerController.GetLocalControlledEntity())
		{
			GetGame().GetInputManager().AddActionListener("ACE_VerticalZeroingUp", EActionTrigger.DOWN, ACE_VerticalZeroingUp);
			GetGame().GetInputManager().AddActionListener("ACE_VerticalZeroingDown", EActionTrigger.DOWN, ACE_VerticalZeroingDown);
			GetGame().GetInputManager().AddActionListener("ACE_HorizontalZeroingRight", EActionTrigger.DOWN, ACE_HorizontalZeroingRight);
			GetGame().GetInputManager().AddActionListener("ACE_HorizontalZeroingLeft", EActionTrigger.DOWN, ACE_HorizontalZeroingLeft);
		}
		else
		{
			GetGame().GetInputManager().RemoveActionListener("ACE_VerticalZeroingUp", EActionTrigger.DOWN, ACE_VerticalZeroingUp);
			GetGame().GetInputManager().RemoveActionListener("ACE_VerticalZeroingDown", EActionTrigger.DOWN, ACE_VerticalZeroingDown);
			GetGame().GetInputManager().RemoveActionListener("ACE_HorizontalZeroingRight", EActionTrigger.DOWN, ACE_HorizontalZeroingRight);
			GetGame().GetInputManager().RemoveActionListener("ACE_HorizontalZeroingLeft", EActionTrigger.DOWN, ACE_HorizontalZeroingLeft);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_VerticalZeroingUp(float value = 0.0, EActionTrigger reason = 0)
	{
		SCR_2DOpticsComponent optics = SCR_2DOpticsComponent.Cast(GetWeaponManagerComponent().GetCurrentSights());
		if (optics)
			optics.ACE_AdjustZeroings(verticalChange: 0.1);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_VerticalZeroingDown(float value = 0.0, EActionTrigger reason = 0)
	{
		SCR_2DOpticsComponent optics = SCR_2DOpticsComponent.Cast(GetWeaponManagerComponent().GetCurrentSights());
		if (optics)
			optics.ACE_AdjustZeroings(verticalChange: -0.1);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_HorizontalZeroingRight(float value = 0.0, EActionTrigger reason = 0)
	{
		SCR_2DOpticsComponent optics = SCR_2DOpticsComponent.Cast(GetWeaponManagerComponent().GetCurrentSights());
		if (optics)
			optics.ACE_AdjustZeroings(horizontalChange: 0.1);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_HorizontalZeroingLeft(float value = 0.0, EActionTrigger reason = 0)
	{
		SCR_2DOpticsComponent optics = SCR_2DOpticsComponent.Cast(GetWeaponManagerComponent().GetCurrentSights());
		if (optics)
			optics.ACE_AdjustZeroings(horizontalChange: -0.1);
	}
}
