//------------------------------------------------------------------------------------------------
class ACE_InspectGadgetMenu : ChimeraMenuBase
{
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		GetGame().GetInputManager().AddActionListener("MenuOpen", EActionTrigger.DOWN, Close);
		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		super.OnMenuClose();
		GetGame().GetInputManager().RemoveActionListener("MenuOpen", EActionTrigger.DOWN, Close);
		GetGame().GetInputManager().RemoveActionListener("MenuBack", EActionTrigger.DOWN, Close);
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_PhysicalButtonsUIComponent GetPhysicalButtonsUIComponent()
	{
		return ACE_PhysicalButtonsUIComponent.Cast(GetRootWidget().FindHandler(ACE_PhysicalButtonsUIComponent));
	}
}
