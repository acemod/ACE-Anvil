//------------------------------------------------------------------------------------------------
//! Creates radial menu entry for the item passed to constructor
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class ACE_SelectTurretMagazineRadialMenuEntry : ACE_SelectItemRadialMenuEntry
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!char)
			return;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(char.GetCompartmentAccessComponent());
		if (!compartmentAccess)
			return;
		
		TurretCompartmentSlot turretCompartment = TurretCompartmentSlot.Cast(compartmentAccess.GetCompartment());
		if (!turretCompartment)
			return;
		
		SCR_TurretControllerComponent turretController = SCR_TurretControllerComponent.Cast(turretCompartment.GetOwner().FindComponent(SCR_TurretControllerComponent));
		if (!turretController)
			return;
		
		turretController.ACE_ReloadWeaponWith(m_pItem);
	}
}
