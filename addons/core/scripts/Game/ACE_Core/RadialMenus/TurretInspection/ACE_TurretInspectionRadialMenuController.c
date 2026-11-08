//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_TurretInspectionRadialMenuController : ACE_RadialMenuController
{
	//------------------------------------------------------------------------------------------------
	override void OnInputOpen()
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!char)
			return;
		
		CompartmentAccessComponent compartmentAccess = char.GetCompartmentAccessComponent();
		if (!compartmentAccess || !TurretCompartmentSlot.Cast(compartmentAccess.GetCompartment()))
			return;
		
		super.OnInputOpen();
	}
}
